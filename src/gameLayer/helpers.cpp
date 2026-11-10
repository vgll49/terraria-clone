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