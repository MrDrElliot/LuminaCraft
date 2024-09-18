#include "Chunk.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Block.h"
#include "World.h"
#include "WorldGen.h"
#include "../Application.h"
#include "../Logging/Log.h"
#include "../Renderer/Renderer.h"

Chunk::Chunk(uint8_t chunkSize, glm::ivec3 chunkPos)
{
	this->chunkSize = chunkSize;
	this->chunkPos = chunkPos;
	worldPos = glm::vec3(chunkPos.x * chunkSize, chunkPos.y * chunkSize, chunkPos.z * chunkSize);

	ready = false;
	
	Future = Application::GetThreadPool()->submit_task([this]
	{
		GenerateChunk();
	});
}

Chunk::~Chunk()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void Chunk::GenerateChunk()
{
	// Estimate number of faces and preallocate memory
	vertices.reserve(chunkSize * chunkSize * chunkSize * 6 * 4);
	indices.reserve(chunkSize * chunkSize * chunkSize * 6 * 6);

	// Generate the chunk's block data and adjacent chunk data
	WorldGen::GenerateChunkData(chunkPos.x, chunkPos.y, chunkPos.z, chunkSize, &BlockData);
	std::vector<uint8_t> northData, southData, eastData, westData, upData, downData;

	WorldGen::GenerateChunkData(chunkPos.x,     chunkPos.y,     chunkPos.z - 1, chunkSize, &northData);
	WorldGen::GenerateChunkData(chunkPos.x,     chunkPos.y,     chunkPos.z + 1, chunkSize, &southData);
	WorldGen::GenerateChunkData(chunkPos.x + 1, chunkPos.y,     chunkPos.z, chunkSize, &eastData);
	WorldGen::GenerateChunkData(chunkPos.x - 1, chunkPos.y,     chunkPos.z, chunkSize, &westData);
	WorldGen::GenerateChunkData(chunkPos.x,     chunkPos.y + 1, chunkPos.z, chunkSize, &upData);
	WorldGen::GenerateChunkData(chunkPos.x,     chunkPos.y - 1, chunkPos.z, chunkSize, &downData);

	unsigned int currentVertex = 0;
	for (int x = 0; x < chunkSize; x++)
	{
		for (int z = 0; z < chunkSize; z++)
		{
			for (int y = 0; y < chunkSize; y++)
			{
				int index = x * chunkSize * chunkSize + z * chunkSize + y;
				if (BlockData[index] == 0)
				{
					continue;
				}

				const Block& block = BlockDictionary[BlockData[index]];

				// Generate faces
				GenerateFace(x, y, z, block, northData, currentVertex,	EDirection::North);
				GenerateFace(x, y, z, block, southData, currentVertex,	EDirection::South);
				GenerateFace(x, y, z, block, westData,	currentVertex,	EDirection::West);
				GenerateFace(x, y, z, block, eastData,	currentVertex,	EDirection::East);
				GenerateFace(x, y, z, block, downData,	currentVertex,	EDirection::Bottom);
				GenerateFace(x, y, z, block, upData,	currentVertex,	EDirection::Top);
			}
		}
	}
}


void Chunk::Render(int modelLoc)
{
	if (!ready)
	{
		if (Future._Is_ready())
		{
			numTriangles = indices.size();

			Renderer::CreateBuffers(vertices, indices, vao, vbo, ebo);
			
			ready = true;
		}
		return;
	}


	// Model transformation: translating the chunk to its correct world position
	glm::mat4 model = glm::mat4(1.0f);
	model = translate(model, static_cast<glm::vec3>(worldPos));

	Renderer::DrawIndexed(modelLoc, model, vao, numTriangles);
}

bool Chunk::IsFaceVisible(int x, int y, int z, const std::vector<uint8_t>& blockData, const std::vector<uint8_t>& adjacentData, EDirection direction, int chunkSize)
{
	int adjacentBlock = 0;

	switch (direction)
	{
	case EDirection::North:
		adjacentBlock = (z > 0) ? blockData[x * chunkSize * chunkSize + (z - 1) * chunkSize + y] : adjacentData[x * chunkSize * chunkSize + (chunkSize - 1) * chunkSize + y];
		break;
	case EDirection::South:
		adjacentBlock = (z < chunkSize - 1) ? blockData[x * chunkSize * chunkSize + (z + 1) * chunkSize + y] : adjacentData[x * chunkSize * chunkSize + 0 * chunkSize + y];
		break;
	case EDirection::West:
		adjacentBlock = (x > 0) ? blockData[(x - 1) * chunkSize * chunkSize + z * chunkSize + y] : adjacentData[(chunkSize - 1) * chunkSize * chunkSize + z * chunkSize + y];
		break;
	case EDirection::East:
		adjacentBlock = (x < chunkSize - 1) ? blockData[(x + 1) * chunkSize * chunkSize + z * chunkSize + y] : adjacentData[0 * chunkSize * chunkSize + z * chunkSize + y];
		break;
	case EDirection::Bottom:
		adjacentBlock = (y > 0) ? blockData[x * chunkSize * chunkSize + z * chunkSize + (y - 1)] : adjacentData[x * chunkSize * chunkSize + z * chunkSize + (chunkSize - 1)];
		break;
	case EDirection::Top:
		adjacentBlock = (y < chunkSize - 1) ? blockData[x * chunkSize * chunkSize + z * chunkSize + (y + 1)] : adjacentData[x * chunkSize * chunkSize + z * chunkSize + 0];
		break;
	}

	return adjacentBlock == 0;
}

void Chunk::GenerateFace(int x, int y, int z, const Block& block, const std::vector<uint8_t>& adjacentData, unsigned& currentVertex, EDirection direction)
{
	if (!IsFaceVisible(x, y, z, BlockData, adjacentData, direction, chunkSize))
		return;

	// Add vertices based on direction
	switch (direction)
	{
	case EDirection::North:
		AddFaceVertices(x + 1, y + 0, z + 0, x + 0, y + 0, z + 0, x + 1, y + 1, z + 0, x + 0, y + 1, z + 0, block.sideMinX, block.sideMinY, block.sideMaxX, block.sideMaxY, currentVertex);
		break;
	case EDirection::South:
		AddFaceVertices(x + 0, y + 0, z + 1, x + 1, y + 0, z + 1, x + 0, y + 1, z + 1, x + 1, y + 1, z + 1, block.sideMinX, block.sideMinY, block.sideMaxX, block.sideMaxY, currentVertex);
		break;
	case EDirection::West:
		AddFaceVertices(x + 0, y + 0, z + 0, x + 0, y + 0, z + 1, x + 0, y + 1, z + 0, x + 0, y + 1, z + 1, block.sideMinX, block.sideMinY, block.sideMaxX, block.sideMaxY, currentVertex);
		break;
	case EDirection::East:
		AddFaceVertices(x + 1, y + 0, z + 1, x + 1, y + 0, z + 0, x + 1, y + 1, z + 1, x + 1, y + 1, z + 0, block.sideMinX, block.sideMinY, block.sideMaxX, block.sideMaxY, currentVertex);
		break;
	case EDirection::Bottom:
		AddFaceVertices(x + 1, y + 0, z + 1, x + 0, y + 0, z + 1, x + 1, y + 0, z + 0, x + 0, y + 0, z + 0, block.bottomMinX, block.bottomMinY, block.bottomMaxX, block.bottomMaxY, currentVertex);
		break;
	case EDirection::Top:
		AddFaceVertices(x + 0, y + 1, z + 1, x + 1, y + 1, z + 1, x + 0, y + 1, z + 0, x + 1, y + 1, z + 0, block.topMinX, block.topMinY, block.topMaxX, block.topMaxY, currentVertex);
		break;
	}
}

void Chunk::AddFaceVertices(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4, float uMin, float vMin, float uMax, float vMax, unsigned& currentVertex)
{
	vertices.emplace_back(x1, y1, z1, uMin, vMin);
	vertices.emplace_back(x2, y2, z2, uMax, vMin);
	vertices.emplace_back(x3, y3, z3, uMin, vMax);
	vertices.emplace_back(x4, y4, z4, uMax, vMax);

	indices.push_back(currentVertex + 0);
	indices.push_back(currentVertex + 3);
	indices.push_back(currentVertex + 1);
	indices.push_back(currentVertex + 0);
	indices.push_back(currentVertex + 2);
	indices.push_back(currentVertex + 3);

	currentVertex += 4;
}

uint8_t Chunk::GetBlockAtPosition(const glm::ivec3 Pos) const
{
	if (Pos.x < 0 || Pos.x >= chunkSize || Pos.y < 0 || Pos.y >= chunkSize || Pos.z < 0 || Pos.z >= chunkSize)
	{
		return -1;
	}

	const size_t index = static_cast<size_t>(Pos.x) +
						 static_cast<size_t>(Pos.y) * chunkSize +
						 static_cast<size_t>(Pos.z) * chunkSize * chunkSize;

	if (index >= BlockData.size())
	{
		return -1;
	}

	return BlockData[index];
}

glm::ivec3 Chunk::WorldToChunkCoords(const glm::vec3& worldPosition, uint8_t chunkSize)
{
	int chunkX = static_cast<int>(std::floor(worldPosition.x / chunkSize));
	int chunkY = static_cast<int>(std::floor(worldPosition.y / chunkSize));
	int chunkZ = static_cast<int>(std::floor(worldPosition.z / chunkSize));

	return {chunkX, chunkY, chunkZ};
}

glm::vec3 Chunk::WorldToLocalChunkCoords(const glm::vec3& worldPosition, const glm::ivec3& chunkPos, uint8_t chunkSize)
{
	return
	{
		worldPosition.x - chunkPos.x * chunkSize,
		worldPosition.y - chunkPos.y * chunkSize,
		worldPosition.z - chunkPos.z * chunkSize
	};
}

