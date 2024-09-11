#include "WorldGen.h"

#include <OpenSimplexNoise.hh>
#include <vector>

#include <glm/glm.hpp>

#include "Block.h"

void WorldGen::GenerateChunkData(int chunkX, int chunkY, int chunkZ, int chunkSize, std::vector<uint8_t>* chunkData)
{
    chunkData->reserve(chunkSize * chunkSize * chunkSize);

    // Noise generators
    OSN::Noise<2> surfaceNoise;
    OSN::Noise<2> biomeNoise;
    OSN::Noise<3> caveNoise;

    int startX = chunkX * chunkSize;
    int startY = chunkY * chunkSize;
    int startZ = chunkZ * chunkSize;

    for (int x = 0; x < chunkSize; x++)
    {
        for (int z = 0; z < chunkSize; z++)
        {
            // Height calculation
            float surfaceNoiseValue = surfaceNoise.eval((float)(x + startX) / 32, (float)(z + startZ) * 0.03f);

            int noiseY = static_cast<int>((surfaceNoiseValue + 1.0f) * 0.5f * 10.0f + 32.0f);

            // Biome noise
            float biomeNoiseValue = biomeNoise.eval((float)(x + startX) * 0.1f, (float)(z + startZ) * 0.2f);
            int biomeType = biomeNoiseValue > 0.0f ? 1 : 0; // Simple biome switch

            for (int y = 0; y < chunkSize; y++)
            {
                float caveNoiseValue = caveNoise.eval
                (
                    static_cast<float>(x + startX) * 0.1f,
                    static_cast<float>(y + startY) * 0.1f,
                    static_cast<float>(z + startZ) * 0.1f
                );

                if (y + startY > noiseY || caveNoiseValue > 0.5f)
                {
                    chunkData->push_back((uint8_t)Block::EBlockType::AIR);
                }
                else if (y + startY == noiseY)
                {
                    chunkData->push_back((uint8_t)Block::EBlockType::GRASS);
                }
                else if (y + startY > noiseY - 5)
                {
                    chunkData->push_back(biomeType == 0 ? (uint8_t)Block::EBlockType::GRASS : (uint8_t)Block::EBlockType::DIRT);
                }
                else
                {
                    chunkData->push_back((uint8_t)Block::EBlockType::STONE);
                }
            }
        }
    }
}
