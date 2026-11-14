#pragma once
#include <vector>
#include <blocks.h>

enum class MapLayer
{
    Foreground,
    Background
};
struct GameMap
{
    int w = 0;
    int h = 0;

    std::vector<Block> mapData;
    std::vector<Block> backgroundMapData;

    void create(int w, int h);

    Block &getBlockUnsafe(
        int x,
        int y,
        MapLayer layer = MapLayer::Foreground // foreground by default
    );
    
    Block *getBlockSafe(
        int x, 
        int y, 
        MapLayer layer = MapLayer::Foreground // foreground by default
    );
};

