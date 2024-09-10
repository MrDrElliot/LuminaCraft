#include "Block.h"

#include <algorithm>

Block::Block(char minX, char minY, char maxX, char maxY, Blocks::EBlockType InType)
{
	BlockType = InType;
	
	topMinX = minX;
	topMinY = minY;
	topMaxX = maxX;
	topMaxY = maxY;

	bottomMinX = minX;
	bottomMinY = minY;
	bottomMaxX = maxX;
	bottomMaxY = maxY;

	sideMinX = minX;
	sideMinY = minY;
	sideMaxX = maxX;
	sideMaxY = maxY;
}

Block::Block(char topMinX, char topMinY, char topMaxX, char topMaxY,
	char bottomMinX, char bottomMinY, char bottomMaxX, char bottomMaxY,
	char sideMinX, char sideMinY, char sideMaxX, char sideMaxY, Blocks::EBlockType InType)
{
	BlockType = InType;
	this->topMinX = topMinX;
	this->topMinY = topMinY;
	this->topMaxX = topMaxX;
	this->topMaxY = topMaxY;

	this->bottomMinX = bottomMinX;
	this->bottomMinY = bottomMinY;
	this->bottomMaxX = bottomMaxX;
	this->bottomMaxY = bottomMaxY;

	this->sideMinX = sideMinX;
	this->sideMinY = sideMinY;
	this->sideMaxX = sideMaxX;
	this->sideMaxY = sideMaxY;
}

Collision::Box Block::GetAABB() const
{
	// Compute the overall minimums and maximums
	float minX = std::min({topMinX, bottomMinX, sideMinX});
	float minY = std::min({topMinY, bottomMinY, sideMinY});
	float maxX = std::max({topMaxX, bottomMaxX, sideMaxX});
	float maxY = std::max({topMaxY, bottomMaxY, sideMaxY});

	// Calculate width and height based on the min and max values
	float width = maxX - minX;
	float height = maxY - minY;

	// Create and return the AABB (Box)
	Collision::Box box;
	box.x = minX;
	box.y = minY;
	box.w = width;
	box.h = height;
	box.vx = 0.0f; // Assuming the block is static (no velocity)
	box.vy = 0.0f;

	return box;
}
