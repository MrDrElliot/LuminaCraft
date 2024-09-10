#pragma once

#include <unordered_map>
#include <string>
#include <queue>
#include <glm/glm.hpp>

#include "Chunk.h"
#include "../TupleHash.h"
#include "Camera.h"

class Planet
{

public:

	Planet(std::string InPlanetName);
	~Planet();


	static std::shared_ptr<Planet> CreatePlanet(const std::string& InPlanetName);

	
	void Update(double DeltaTime);

public:

	unsigned int numChunks = 0, numChunksRendered = 0;

private:

	std::string PlanetName;

	std::shared_ptr<Camera> MainCamera;

	std::unordered_map<std::tuple<int, int, int>, Chunk> chunks;
	std::queue<glm::vec3> chunkQueue;
	int renderDistance = 10;
	int renderHeight = 1;
	unsigned int chunkSize = 32;
	unsigned int chunksLoading = 0;
	int lastCamX = -100, lastCamY = -100, lastCamZ = -100;
};