#pragma once
#include "../Physics/Collision/Box.h"

namespace Blocks
{
	enum EBlockType : int;
}

struct Block
{
public:

	Block(char minX, char minY, char maxX, char maxY, Blocks::EBlockType InType);
	Block(char topMinX, char topMinY, char topMaxX, char topMaxY, 
		char bottomMinX, char bottomMinY, char bottomMaxX, char bottomMaxY,
		char sideMinX, char sideMinY, char sideMaxX, char sideMaxY, Blocks::EBlockType InType);

	// Get the AABB bounding box for this block
	[[nodiscard]] Collision::Box GetAABB() const;

	char topMinX, topMinY, topMaxX, topMaxY;
	char bottomMinX, bottomMinY, bottomMaxX, bottomMaxY;
	char sideMinX, sideMinY, sideMaxX, sideMaxY;

	Blocks::EBlockType BlockType;
};
