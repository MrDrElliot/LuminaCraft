#include "Chunk.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Renderer/Vertex.h"
#include "Block.h"
#include "World.h"
#include "WorldGen.h"
#include "../Logging/Log.h"

Chunk::Chunk(uint8_t chunkSize, glm::ivec3 chunkPos)
{
	this->chunkSize = chunkSize;
	this->chunkPos = chunkPos;
	worldPos = glm::vec3(chunkPos.x * chunkSize, chunkPos.y * chunkSize, chunkPos.z * chunkSize);

	ready = false;
	generated = false;
	chunkThread = std::thread(&Chunk::GenerateChunk, this);
}

Chunk::~Chunk()
{
	if (chunkThread.joinable())
	{
		chunkThread.join();
	}

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vertexArrayObject);
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
				GenerateFace(x, y, z, block, northData, currentVertex, EDirection::North);
				GenerateFace(x, y, z, block, southData, currentVertex, EDirection::South);
				GenerateFace(x, y, z, block, westData, currentVertex, EDirection::West);
				GenerateFace(x, y, z, block, eastData, currentVertex, EDirection::East);
				GenerateFace(x, y, z, block, downData, currentVertex, EDirection::Bottom);
				GenerateFace(x, y, z, block, upData, currentVertex, EDirection::Top);
			}
		}
	}

	generated = true;
}


void Chunk::Render(unsigned int modelLoc)
{
	if (!ready)
	{
		if (generated)
		{
			numTriangles = indices.size();

			glGenVertexArrays(1, &vertexArrayObject);
			glBindVertexArray(vertexArrayObject);

			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, posX));
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texGridX));
			glEnableVertexAttribArray(1);

			glGenBuffers(1, &ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

			ready = true;
		}
		return;
	}

	// Bind VAO to render the chunk
	glBindVertexArray(vertexArrayObject);

	// Model transformation: translating the chunk to its correct world position
	glm::mat4 model = glm::translate(glm::mat4(1.0f), (glm::vec3)worldPos);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draw the chunk using the indices (triangles)
	glDrawElements(GL_TRIANGLES, numTriangles, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

