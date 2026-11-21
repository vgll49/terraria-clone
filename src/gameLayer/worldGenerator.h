#pragma once
#include "gameMap.h"
#include "blocks.h"

// enum class BiomeType {
//     Plains,
//     Mountains,
//     Desert,
// };

// struct BiomeDefinition {
//     BiomeType type;
//     int minSize;
//     int maxSize;

//     Block surface;
//     Block underground;
//     Block deepUnderground;

// };

// struct Biome {
//     BiomeType type;
//     int startX;
//     int size;

//     Block surface;
//     Block underground;
//     Block deepUnderground;
// };

void generateWorld(GameMap &gameMap, int seed = 1234);