#pragma once 

#include <raylib.h>
#include <cstdint>

Rectangle getTextureAtlas(int x, int y, int cellSizePixelsX, int cellSizePixelsY);
std::uint64_t hashCoordinates(int x, int y);
