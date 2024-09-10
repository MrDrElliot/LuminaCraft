#pragma once

#include <vector>
#include <thread>
#include <unordered_map>
#include <glm/glm.hpp>

struct Block;

struct Vertex
{
	char posX, posY, posZ;
	char texGridX, texGridY;

	Vertex(char _posX, char _posY, char _posZ, char _texGridX, char _texGridY)
	{
		posX = _posX;
		posY = _posY;
		posZ = _posZ;

		texGridX = _texGridX;
		texGridY = _texGridY;
	}
};

class Chunk
{
public:
	Chunk(unsigned int chunkSize, glm::vec3 chunkPos);
	~Chunk();

	void GenerateChunk();
	void Render(unsigned int modelLoc);
	

public:
	std::vector<uint32_t> chunkData;
	glm::ivec3 chunkPos;
	bool ready;
	bool generated;

private:
	unsigned int vertexArrayObject;
	unsigned int vbo, ebo;
	uint32_t chunkSize;
	unsigned long long numTriangles;
	
	glm::ivec3 worldPos;
	std::thread chunkThread;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};