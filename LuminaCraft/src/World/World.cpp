#include "World.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Logging/Log.h"
#include "../Renderer/ShaderLibrary.h"
#include <glad/glad.h>
#include "../Application.h"
#include "../Player/Player.h"

World::World(std::string InWorldName)
	:WorldName(std::move(InWorldName))
{
	m_Player = std::make_shared<Player>(this);
}

World::~World()
{
    chunkQueue.empty();
    chunks.empty();
}

std::shared_ptr<World> World::CreateWorld(const std::string& InWorldName)
{
	LOG_INFO("Creating World: {0}", InWorldName);
	return std::make_shared<World>(InWorldName);
}


void World::Update(double DeltaTime)
{
    m_Player->Update(DeltaTime);
    std::shared_ptr<Camera> Camera = m_Player->GetCamera();

    glm::mat4 view = Camera->GetViewMatrix();
    glm::mat4 projection = Camera->GetProjectionMatrix();

    std::shared_ptr<Shader> PrimaryShader = ShaderLibrary::GetShader("Primary");

    const int viewLoc = glGetUniformLocation(PrimaryShader->GetProgramID(), "view");
    const int projectionLoc = glGetUniformLocation(PrimaryShader->GetProgramID(), "projection");
    const int modelLoc = glGetUniformLocation(PrimaryShader->GetProgramID(), "model");

    static glm::mat4 lastView = glm::mat4(1.0f);
    static glm::mat4 lastProjection = glm::mat4(1.0f);

    if (view != lastView)
    {
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        lastView = view;
    }
    if (projection != lastProjection)
    {
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        lastProjection = projection;
    }

    glm::vec3 camPos = Camera->GetPosition();
    int camChunkX = static_cast<int>(std::floor(camPos.x / chunkSize));
    int camChunkY = static_cast<int>(std::floor(camPos.y / chunkSize));
    int camChunkZ = static_cast<int>(std::floor(camPos.z / chunkSize));

    if (camChunkX != lastCamX || camChunkY != lastCamY || camChunkZ != lastCamZ)
    {
        lastCamX = camChunkX;
        lastCamY = camChunkY;
        lastCamZ = camChunkZ;

        chunkQueue = {};
        if (!chunks.contains({ camChunkX, camChunkY, camChunkZ }))
        {
            chunkQueue.emplace(camChunkX, camChunkY, camChunkZ);
        }

        // Circular chunk generation
        for (int r = 0; r <= renderDistance; r++)
        {
            for (int x = -r; x <= r; x++)
            {
                for (int z = -r; z <= r; z++)
                {
                    if (x * x + z * z <= r * r)
                    {
                        for (int y = 0; y <= renderHeight; y++)
                        {
                            chunkQueue.emplace(camChunkX + x, y, camChunkZ + z);
                            chunkQueue.emplace(camChunkX + x, -y, camChunkZ + z);
                        }
                    }
                }
            }
        }
    }
    else if (chunksLoading == 0 && !chunkQueue.empty())
    {
        glm::vec3 next = chunkQueue.front();
        chunkQueue.pop();

        const std::tuple<int, int, int> chunkTuple{ next.x, next.y, next.z };

        if (!chunks.contains(chunkTuple))
        {
            chunks.try_emplace(chunkTuple, std::make_shared<Chunk>(chunkSize, next));
        }
    }

    chunksLoading = 0;
    numChunks = 0;
    numChunksRendered = 0;
    for (auto it = chunks.begin(); it != chunks.end();)
    {
        numChunks++;
        if (!it->second->ready)
        {
            chunksLoading++;
        }
        if (it->second->ready && (glm::distance(glm::vec3(it->second->chunkPos.x, it->second->chunkPos.y, it->second->chunkPos.z), glm::vec3(camChunkX, camChunkY, camChunkZ)) > renderDistance))
        {
            it = chunks.erase(it);
        }
        else
        {
            numChunksRendered++;
            it->second->Render(modelLoc);
            ++it;
        }

    }
    
}

std::shared_ptr<Player> World::GetPlayer() const
{
    return m_Player;
}

std::shared_ptr<Chunk> World::GetChunkAtPosition(const glm::vec3& worldPos) const
{
    // Calculate the chunk coordinates from the world position
    int chunkX = static_cast<int>(std::floor(worldPos.x / chunkSize));
    int chunkY = static_cast<int>(std::floor(worldPos.y / chunkSize));
    int chunkZ = static_cast<int>(std::floor(worldPos.z / chunkSize));

    // Create a tuple to represent the chunk coordinates
    std::tuple<int, int, int> chunkCoord(chunkX, chunkY, chunkZ);

    auto it = chunks.find(chunkCoord);
    if (it != chunks.end())
    {
        return it->second;
    }
        
    return nullptr; 
}

uint8_t World::GetBlockAtWorldPosition(const glm::vec3& worldPosition) const
{
    // Get the chunk at the given world position
    if (const auto chunk = GetChunkAtPosition(worldPosition))
    {
        // Convert world position to local chunk coordinates
        glm::ivec3 chunkPos = chunk->chunkPos;
        glm::vec3 localPos = Chunk::WorldToLocalChunkCoords(worldPosition, chunkPos, chunkSize);
        glm::ivec3 localChunkPos = glm::ivec3(localPos);

        // Ensure localChunkPos is within chunk bounds
        if (localChunkPos.x >= 0 && localChunkPos.x < chunkSize &&
            localChunkPos.y >= 0 && localChunkPos.y < chunkSize &&
            localChunkPos.z >= 0 && localChunkPos.z < chunkSize)
        {
            // Get the block at the local position within the chunk
            return chunk->GetBlockAtPosition(localChunkPos);
        }
    }
    return -1; // Return a default value or handle as needed
}

