#pragma once

#include <unordered_map>
#include <string>
#include <queue>
#include <glm/glm.hpp>

#include "Chunk.h"
#include "../TupleHash.h"
#include "Camera.h"

struct Block;
class Player;

class World
{

public:

	World(std::string InWorldName);
	~World();


	static std::shared_ptr<World> CreateWorld(const std::string& InWorldName);

	
	void Update(double DeltaTime);

public:

	unsigned int numChunks = 0;
	unsigned int numChunksRendered = 0;

private:

	std::string WorldName;

	std::shared_ptr<Player> m_Player;

	std::unordered_map<std::tuple<int, int, int>, Chunk> chunks;
	std::queue<glm::vec3> chunkQueue;
	int renderDistance = 16;
	int renderHeight = 1;
	unsigned int chunkSize = 32;
	unsigned int chunksLoading = 0;
	int lastCamX = -100, lastCamY = -100, lastCamZ = -100;
};
