#include "worldGenerator.h"
#include "randomStuff.h"
#include <algorithm>
#include <iostream>

void generateWorld(GameMap &gameMap, int seed) {
    const int w = 900;
    const int h = 500;

    gameMap.create(w, h);


    
    std::ranlux24_base rng(seed);
    int dirtBase = 70;
    int stoneBase = 90;

    int dirtHeight = dirtBase;
    int stoneHeight = stoneBase;

for(int x = 0; x < w; x++)
{
    dirtHeight += getRandomInt(rng, -1, 1);
    stoneHeight += getRandomInt(rng, -1, 1);

    dirtHeight = std::clamp(dirtHeight, dirtBase - 15, dirtBase + 15);
    stoneHeight = std::clamp(stoneHeight, stoneBase - 25, stoneBase + 25);

        for(int y = 0; y < h; y++)
        {
            Block b;

            if(y > dirtHeight) {
               b.type = Block::dirt;
            } 
            if(y == dirtHeight) {
                b.type = Block::grassBlock;
            }
            
            if(y > stoneHeight) {
                b.type = Block::stone;
            }

            gameMap.getBlockUnsafe(x, y) = b;
        }

    }
}