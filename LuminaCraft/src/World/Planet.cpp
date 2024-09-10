#include "Planet.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Logging/Log.h"
#include "../Renderer/ShaderLibrary.h"
#include <glad/glad.h>

#include "../Application.h"

Planet::Planet(std::string InPlanetName)
	:PlanetName(std::move(InPlanetName))
{
	MainCamera = std::make_shared<Camera>();
}

Planet::~Planet()
{

}

std::shared_ptr<Planet> Planet::CreatePlanet(const std::string& InPlanetName)
{
	LOG_INFO("Creating Planet: {0}", InPlanetName);
	return std::make_shared<Planet>(InPlanetName);
}


void Planet::Update(double DeltaTime)
{
	MainCamera->Update(DeltaTime);

	glm::mat4 view = MainCamera->GetViewMatrix();
	glm::mat4 projection = MainCamera->GetProjectionMatrix();
    
	const int viewLoc = glGetUniformLocation(ShaderLibrary::GetShader("Primary")->GetProgramID(), "view");
	const int projectionLoc = glGetUniformLocation(ShaderLibrary::GetShader("Primary")->GetProgramID(), "projection");
	const int modelLoc = glGetUniformLocation(ShaderLibrary::GetShader("Primary")->GetProgramID(), "model");

	// Upload view and projection matrices
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Determine the current chunk position based on the camera
	glm::vec3 camPos = MainCamera->GetPosition();
	int camChunkX = static_cast<int>(std::floor(camPos.x / chunkSize));
	int camChunkY = static_cast<int>(std::floor(camPos.y / chunkSize));
	int camChunkZ = static_cast<int>(std::floor(camPos.z / chunkSize));

	// Check if camera moved to new chunk
	if (camChunkX != lastCamX || camChunkY != lastCamY || camChunkZ != lastCamZ)
	{
		// Player moved chunks, start new chunk queue
		lastCamX = camChunkX;
		lastCamY = camChunkY;
		lastCamZ = camChunkZ;

		// Current chunk
		chunkQueue = {};
		if (!chunks.contains({ camChunkX, camChunkY, camChunkZ }))
		{
			chunkQueue.emplace(camChunkX, camChunkY, camChunkZ);
		}

		for (int r = 0; r < renderDistance; r++)
		{
			// Add middle chunks
			for (int y = 0; y <= renderHeight; y++)
			{
				chunkQueue.emplace(camChunkX,     y, camChunkZ + r);
				chunkQueue.emplace(camChunkX + r, y, camChunkZ);
				chunkQueue.emplace(camChunkX,     y, camChunkZ - r);
				chunkQueue.emplace(camChunkX - r, y, camChunkZ);

				if (y > 0)
				{
					chunkQueue.emplace(camChunkX,     -y, camChunkZ + r);
					chunkQueue.emplace(camChunkX + r, -y, camChunkZ);
					chunkQueue.emplace(camChunkX,     -y, camChunkZ - r);
					chunkQueue.emplace(camChunkX - r, -y, camChunkZ);
				}
			}

			// Add edges
			for (int e = 1; e < r; e++)
			{
				for (int y = 0; y <= renderHeight; y++)
				{
					chunkQueue.emplace(camChunkX + e, y, camChunkZ + r);
					chunkQueue.emplace(camChunkX - e, y, camChunkZ + r);

					chunkQueue.emplace(camChunkX + r, y, camChunkZ + e);
					chunkQueue.emplace(camChunkX + r, y, camChunkZ - e);

					chunkQueue.emplace(camChunkX + e, y, camChunkZ - r);
					chunkQueue.emplace(camChunkX - e, y, camChunkZ - r);

					chunkQueue.emplace(camChunkX - r, y, camChunkZ + e);
					chunkQueue.emplace(camChunkX - r, y, camChunkZ - e);

					if (y > 0)
					{
						chunkQueue.emplace(camChunkX + e, -y, camChunkZ + r);
						chunkQueue.emplace(camChunkX - e, -y, camChunkZ + r);

						chunkQueue.emplace(camChunkX + r, -y, camChunkZ + e);
						chunkQueue.emplace(camChunkX + r, -y, camChunkZ - e);

						chunkQueue.emplace(camChunkX + e, -y, camChunkZ - r);
						chunkQueue.emplace(camChunkX - e, -y, camChunkZ - r);

						chunkQueue.emplace(camChunkX - r, -y, camChunkZ + e);
						chunkQueue.emplace(camChunkX - r, -y, camChunkZ - e);
					}
				}
			}

			// Add corners
			for (int y = 0; y <= renderHeight; y++)
			{
				chunkQueue.emplace(camChunkX + r, y, camChunkZ + r);
				chunkQueue.emplace(camChunkX + r, y, camChunkZ - r);
				chunkQueue.emplace(camChunkX - r, y, camChunkZ + r);
				chunkQueue.emplace(camChunkX - r, y, camChunkZ - r);

				if (y > 0)
				{
					chunkQueue.emplace(camChunkX + r, -y, camChunkZ + r);
					chunkQueue.emplace(camChunkX + r, -y, camChunkZ - r);
					chunkQueue.emplace(camChunkX - r, -y, camChunkZ + r);
					chunkQueue.emplace(camChunkX - r, -y, camChunkZ - r);
				}
			}
		}
	}
	else if (chunksLoading == 0 && !chunkQueue.empty())
	{
		// Queue is not empty. Process front item in queue
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
	for (auto it = chunks.begin(); it != chunks.end(); )
	{
		numChunks++;

		if (!it->second.ready)
			chunksLoading++;

		int chunkX = it->second.chunkPos.x;
		int chunkY = it->second.chunkPos.y;
		int chunkZ = it->second.chunkPos.z;
		if (it->second.ready && (abs(chunkX - camChunkX) > renderDistance ||
			abs(chunkY - camChunkY) > renderDistance ||
			abs(chunkZ - camChunkZ) > renderDistance))
		{
			it->second.~Chunk();
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