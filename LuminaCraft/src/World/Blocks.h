#pragma once

#include "Block.h"

namespace Blocks
{
	enum EBlockType
	{
		AIR = 0,
		DIRT = 1,
		GRASS = 2,
		STONE = 3
	};

	const Block blockDefinitions[4] = {
		Block(0, 0, 0, 0, EBlockType::AIR),   // Air
		Block(0, 0, 1, 1, EBlockType::DIRT),  // Dirt
		Block(1, 1, 2, 2,                    // Grass
			  0, 0, 1, 1,
			  1, 0, 2, 1, EBlockType::GRASS),
		Block(0, 1, 1, 2, EBlockType::STONE)  // Stone
	};

	// Utility function to get Block by type
	inline const Block& GetBlock(EBlockType blockType)
	{
		return blockDefinitions[static_cast<unsigned int>(blockType)];
	}
}
