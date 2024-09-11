#pragma once

#include <unordered_map>
#include <string>
#include <queue>
#include <glm/glm.hpp>

#include "Chunk.h"
#include "../TupleHash.h"
#include "Camera.h"

struct DebugLine;
struct Block;
class Player;

class World
{

public:

	World(std::string InWorldName);
	~World();


	static std::shared_ptr<World> CreateWorld(const std::string& InWorldName);

	
	void Update(double DeltaTime);

	std::shared_ptr<Player> GetPlayer() const;
	
	std::shared_ptr<Chunk> GetChunkAtPosition(const glm::vec3& worldPos) const;
	uint8_t GetBlockAtWorldPosition(const glm::vec3& worldPosition) const;

	uint8_t Raycast(const glm::vec3& start, const glm::vec3& end, glm::ivec3& hitBlockPos, float maxDistance = 100.0f);

	void UpdateDebugLines(float deltaTime);
	void RenderDebugLines();
	void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);


public:

	uint32_t numChunks = 0;
	uint32_t numChunksRendered = 0;

private:

	/** Name of the world */
	std::string WorldName;

	/** Player */
	std::shared_ptr<Player> m_Player;

	/** All chunks currently in memory */
	std::unordered_map<std::tuple<int, int, int>, std::shared_ptr<Chunk>> chunks;

	/** Chunks awaiting render */
	std::queue<glm::vec3> chunkQueue;

	/** Render Distance */
	int renderDistance = 6;

	/** Render Height */
	int renderHeight = 4;

	/** How large the chunk is as 3x3 */
	uint8_t chunkSize = 32;

	/** Number of chunks currently loading */
	uint32_t chunksLoading = 0;

	/** Currently rendered debug lines */
	std::vector<DebugLine> debugLines;
	unsigned int lineVAO, lineVBO;

	
	int lastCamX = -100, lastCamY = -100, lastCamZ = -100;
};
