#pragma once

#include <vector>

namespace WorldGen
{
	void GenerateChunkData(int chunkX, int chunkY, int chunkZ, int chunkSize, std::vector<uint8_t>* chunkData);
}