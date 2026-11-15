#include "helpers.h"

Rectangle getTextureAtlas(int x, int y, int cellsSizePixelsX, int cellsSizePixelsY)
{
    return Rectangle{
        (float)x * cellsSizePixelsX, 
        (float)y * cellsSizePixelsY, 
        (float)cellsSizePixelsX,
        (float)cellsSizePixelsY
    };
}



std::uint64_t hashCoordinates(int x, int y)
{
    std::uint64_t z =
        (std::uint64_t(static_cast<std::uint32_t>(x)) << 32) |
        static_cast<std::uint32_t>(y);

    z += 0x9E3779B97F4A7C15ULL;
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    z ^= z >> 31;

    return z;
}