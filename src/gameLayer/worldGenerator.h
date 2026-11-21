#pragma once
#include "gameMap.h"

enum class BiomeType {
    Plains,
    Mountains,
    Desert,
};

struct BiomeDefinition {
    BiomeType type;
    int minSize;
    int maxSize;

};

struct Biome {
    BiomeType type;
    int startX;
    int actualSize;
};

void generateWorld(GameMap &gameMap, int seed = 1234, int dirtOctaves = 1, float dirtFrequency = 0.02, int stoneOctaves = 4, float stoneFrequency = 0.01);
