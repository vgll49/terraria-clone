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

		BLOCKS_COUNT,

	};

    std::uint16_t type = 0;


};

struct BackgroundBlock
{
    enum
    {
        dirtWall = 0,
        stoneWall,
        woodWall,
       
        BACKGROUND_BLOCKS_COUNT
    };

    std::uint16_t type = 0;
};