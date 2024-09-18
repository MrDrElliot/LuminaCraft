#pragma once

#include <future>
#include <vector>
#include "../Renderer/Vertex.h"
#include <glm/glm.hpp>

struct Block;


class Chunk
{
public:

	enum class EDirection { North, South, East, West, Top, Bottom };

	
	Chunk(uint8_t chunkSize, glm::ivec3 chunkPos);
	~Chunk();

	void GenerateChunk();
	void Render(int modelLoc);

	bool IsFaceVisible(int x, int y, int z, const std::vector<uint8_t>& blockData, const std::vector<uint8_t>& adjacentData, EDirection direction, int chunkSize);

	void GenerateFace(int x, int y, int z, const Block& block, const std::vector<uint8_t>& adjacentData, unsigned int& currentVertex, EDirection direction);
	void AddFaceVertices(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4, float uMin, float vMin, float uMax, float vMax, unsigned int& currentVertex);
	uint8_t GetBlockAtPosition(glm::ivec3 Pos) const;

	static glm::ivec3 WorldToChunkCoords(const glm::vec3& worldPosition, uint8_t chunkSize);
	static glm::vec3 WorldToLocalChunkCoords(const glm::vec3& worldPosition, const glm::ivec3& chunkPos, uint8_t chunkSize);

public:
	
	std::vector<uint8_t> BlockData;
	glm::ivec3 chunkPos;
	
	bool ready;
	std::future<void> Future;


private:
	
	uint32_t vao, vbo, ebo;
	int32_t chunkSize;
	uint64_t numTriangles;
	
	glm::ivec3 worldPos;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};