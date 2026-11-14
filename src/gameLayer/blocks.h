#pragma once
#include <cstdint>

struct Block 
{
    enum
	{
		air = 0,
		dirt,
		grassBlock,
		stone,
		grass,
		sand,
		sandRuby,
		sandStone,
		woodPlank,
		stoneBricks,
		clay,
		woodLog,
		leaves,
		copper,
		iron,
		gold,
		copperBlock,
		ironBlock,
		goldBlock,
		bricks,
		snow,
		ice,
		rubyBlock,
		platform,
		workBench,
		glass,
		furnace,
		painting,
		sappling,
		snowBlueRuby,
		blueRubyBlock,
		door,
		jar,
		table,
		wordrobe,
		bookShelf,
		snowBricks,
		iceTable,
		iceWordrobe,
		iceBookShelf,
		icePlatform,
		sandTable,
		sandWordrobe,
		sandBookShelf,
		sandPlatform,
		woodenChest,
		iceChest,
		sandChest,
		boneChest,
		boneBricks,
		boneBench,
		boneWordrobe,
		boneBookShelf,
		bonePlatform,
		pearlstone,
		mithril,
		cherryLeaves,
		dirtWall,
		stoneWall, 
		woodWall,
		sandStoneWall,
		brickWall, 
		glassWall,  
		copperBlockWall, 
		silverBlockWall, 
		goldBlockWall, 
		snowWall, 
		sandWall, 
		stoneBricksWall, 
		rubyBlockWall, 
		heroglyphWall, 
		blueRubyWall, 
		plankedWall, 
		snowBrickWall, 
		boneBrickWall,
		pearlstoneWall,
		mithrilWall,
		cherryLeavesWall,

		BLOCKS_COUNT,

	};

    std::uint16_t type = 0;


};

inline bool isWallBlock(std::uint16_t type)
{
    switch (type)
    {
        case Block::dirtWall:
        case Block::stoneWall:
        case Block::woodWall:
        case Block::sandStoneWall:
        case Block::brickWall:
        case Block::glassWall:
        case Block::copperBlockWall:
        case Block::silverBlockWall:
        case Block::goldBlockWall:
        case Block::snowWall:
        case Block::sandWall:
        case Block::stoneBricksWall:
        case Block::rubyBlockWall:
        case Block::heroglyphWall:
        case Block::blueRubyWall:
        case Block::plankedWall:
        case Block::snowBrickWall:
        case Block::boneBrickWall:
        case Block::pearlstoneWall:
        case Block::mithrilWall:
        case Block::cherryLeavesWall:
            return true;

        default:
            return false;
    }
}