#include <raylib.h>
#include "gameMain.h"
#include <asserts.h>

#include <iostream>
#include <fstream>


struct GameData 
{
    float positionX = 100;
    float positionY = 100;
}gameData;


bool initGame()
{
    return true;
}

bool updateGame() 
{
    float deltaTime = GetFrameTime();
    if(deltaTime > 1.f / 5) { deltaTime = 1 / 5.f; }

    if(IsKeyDown(KEY_A)) {gameData.positionX -= 200 * deltaTime; }
    if(IsKeyDown(KEY_D)) {gameData.positionX += 200 * deltaTime; }
    if(IsKeyDown(KEY_W)) {gameData.positionY -= 200 * deltaTime; }
    if(IsKeyDown(KEY_S)) {gameData.positionY += 200 * deltaTime; }

    DrawRectangle(gameData.positionX, gameData.positionY, 50, 50, PURPLE);
    return true;
}

void closeGame()
{
    std::ofstream f(RESOURCES_PATH "f.txt");
    f << "\nCLOSED\n";
    f.close();

}