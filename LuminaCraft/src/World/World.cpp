#include "World.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Logging/Log.h"
#include "../Renderer/ShaderLibrary.h"
#include <glad/glad.h>
#include "Blocks.h"
#include "../Application.h"
#include "../Player/Player.h"

World::World(std::string InWorldName)
	:WorldName(std::move(InWorldName))
{
	m_Player = std::make_shared<Player>();
}

World::~World()
{

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

        for (int r = 0; r < renderDistance; ++r)
        {
            for (int y = 0; y <= renderHeight; ++y)
            {
                chunkQueue.emplace(camChunkX + r, y, camChunkZ);
                chunkQueue.emplace(camChunkX - r, y, camChunkZ);
                chunkQueue.emplace(camChunkX, y, camChunkZ + r);
                chunkQueue.emplace(camChunkX, y, camChunkZ - r);

                if (y > 0)
                {
                    chunkQueue.emplace(camChunkX + r, -y, camChunkZ);
                    chunkQueue.emplace(camChunkX - r, -y, camChunkZ);
                    chunkQueue.emplace(camChunkX, -y, camChunkZ + r);
                    chunkQueue.emplace(camChunkX, -y, camChunkZ - r);
                }
            }
        }
    }
    else if (chunksLoading == 0 && !chunkQueue.empty())
    {
        glm::vec3 next = chunkQueue.front();
        chunkQueue.pop();

        std::tuple<int, int, int> chunkTuple{ next.x, next.y, next.z };

        if (!chunks.contains(chunkTuple))
        {
            chunks.try_emplace(chunkTuple, chunkSize, next);
        }
    }

    chunksLoading = 0;
    numChunks = 0;
    numChunksRendered = 0;
    for (auto it = chunks.begin(); it != chunks.end();)
    {
        numChunks++;
        if (!it->second.ready)
            chunksLoading++;

        if (it->second.ready && (abs(it->second.chunkPos.x - camChunkX) > renderDistance ||
                                 abs(it->second.chunkPos.y - camChunkY) > renderDistance ||
                                 abs(it->second.chunkPos.z - camChunkZ) > renderDistance))
        {
            it = chunks.erase(it);
        }
        else
        {
            numChunksRendered++;
            it->second.Render(modelLoc);
            ++it;
        }
    }
}
