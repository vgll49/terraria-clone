#include "gameMap.h"
#include <asserts.h>

void GameMap::create(int w, int h) 
{
    *this = {};
    mapData.resize(w * h); // resize array
    backgroundMapData.resize(w * h);

    this->w = w;
    this->h = h;

    for(auto &e : mapData) { e = {};} // clear block data
    for(auto &e : backgroundMapData) { e = {};} // clear block data
}

Block &GameMap::getBlockUnsafe(int x, int y, MapLayer layer)
{
    permaAssertCommentDevelopement(mapData.size() == w * h, "Map data not initialized");
    permaAssertCommentDevelopement(backgroundMapData.size() == w * h, "backgroundMapData Map data not initialized");

    permaAssertCommentDevelopement(x >= 0 && y >= 0 && x < w && y < h, "getBlockUnsafe out of bounds error");

    int index = x + y * w;

    if(layer == MapLayer::Background) {
        return backgroundMapData[index];
    }

    return mapData[index];

}

Block *GameMap::getBlockSafe(int x, int y, MapLayer layer)
{
    permaAssertCommentDevelopement(mapData.size() == w * h, "Map data not initialized");
    permaAssertCommentDevelopement(backgroundMapData.size() == w * h, "backgroundMapData Map data not initialized");

    if(x < 0 || y < 0 || x >= w || y >= h) { return nullptr; }

    int index = x + y * w;

    if(layer == MapLayer::Background) {
        return &backgroundMapData[index];
    }

    return &mapData[index];
}