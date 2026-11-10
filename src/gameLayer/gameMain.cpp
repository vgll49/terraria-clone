#include <raylib.h>
#include "gameMain.h"
#include <asserts.h>
#include <assetManager.h>

#include <iostream>
#include <fstream>


struct GameData 
{

    Texture dirtTexture;

}gameData;

AssetManager assetManger;

bool initGame()
{
    
    assetManger.loadAll();

    return true;
}

bool updateGame() 
{
    float deltaTime = GetFrameTime();
    if(deltaTime > 1.f / 5) { deltaTime = 1 / 5.f; }

    DrawTexturePro(gameData.dirtTexture, {0,0, (float)gameData.dirtTexture.width, (float)gameData.dirtTexture.height}, {50, 50, 100, 100},  {}, 0, WHITE);

    return true;
}

void closeGame()
{
    std::ofstream f(RESOURCES_PATH "f.txt");
    f << "\nCLOSED\n";
    f.close();

}