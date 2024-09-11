#pragma once

#include <cstdint>
#include <string>

// Block Structure Declaration
struct Block
{
public:

	
	// Enum representing block types
	enum class EBlockType : uint8_t
	{
		AIR = 0,
		DIRT = 1,
		GRASS = 2,
		STONE = 3
	};

	static std::string BlockTypeToString(const EBlockType Type)
	{
		switch (Type)
		{
			case EBlockType::AIR:	return "Air";
			case EBlockType::DIRT:	return "Dirt";
			case EBlockType::GRASS: return "Grass";
			case EBlockType::STONE: return "Stone";
		}
		return "INVALID";
	}

	// Constructors
	Block(char minX, char minY, char maxX, char maxY, EBlockType InType);
	Block(char topMinX, char topMinY, char topMaxX, char topMaxY, 
		  char bottomMinX, char bottomMinY, char bottomMaxX, char bottomMaxY,
		  char sideMinX, char sideMinY, char sideMaxX, char sideMaxY, EBlockType InType);

	// Block Texture Coordinates
	char topMinX, topMinY, topMaxX, topMaxY;
	char bottomMinX, bottomMinY, bottomMaxX, bottomMaxY;
	char sideMinX, sideMinY, sideMaxX, sideMaxY;

private:
	EBlockType BlockType;
};

// Block Dictionary Definition
const Block BlockDictionary[4] =
{
	Block(0, 0, 0, 0, Block::EBlockType::AIR),   // Air
	Block(0, 0, 1, 1, Block::EBlockType::DIRT),  // Dirt
	Block(1, 1, 2, 2,                           // Grass Top
		  0, 0, 1, 1,                           // Grass Bottom
		  1, 0, 2, 1, Block::EBlockType::GRASS), // Grass Side
	Block(0, 1, 1, 2, Block::EBlockType::STONE)  // Stone
};

// Get Block by Type Function
inline Block GetBlock(Block::EBlockType blockType)
{
	return BlockDictionary[static_cast<uint8_t>(blockType)];
}

