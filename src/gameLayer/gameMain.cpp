#include <raylib.h>
#include "gameMain.h"
#include <asserts.h>
#include <assetManager.h>
#include <gameMap.h>
#include <helpers.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <imgui.h>


struct GameData 
{
    GameMap gameMap;
	Camera2D camera;

	Block selected_block;
}gameData;

AssetManager assetManager;

bool initGame()
{

    assetManager.loadAll();

	gameData.gameMap.create(30, 30);
 
    gameData.gameMap.getBlockUnsafe(0, 0).type = Block::dirt;
    gameData.gameMap.getBlockUnsafe(1, 1).type = Block::grass;
    gameData.gameMap.getBlockUnsafe(2, 2).type = Block::goldBlock;
    gameData.gameMap.getBlockUnsafe(3, 3).type = Block::pearlstone;
    gameData.gameMap.getBlockUnsafe(4, 4).type = Block::mithril;

	// Camera
	gameData.camera.target = {0,0}; // world-space center of view
	gameData.camera.rotation = 0.0f;
	gameData.camera.zoom = 100.0f;

    return true;
}

void renderImGui() {
	ImGui::Begin("Block Selector");
        
	if(ImGui::Button("button")) {
		std::cout << "Text\n";
	}

	if(ImGui::Button("button2")) {
		std::cout << "different button\n";
	}
        
    ImGui::InputInt("Block", (int *)&gameData.selected_block.type);

	if (gameData.selected_block.type > Block::BLOCKS_COUNT) 
		gameData.selected_block.type = Block::BLOCKS_COUNT;

	if (gameData.selected_block.type < 1)
		gameData.selected_block.type = 1;

	ImGui::End();
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	if (deltaTime > 1.f / 5) { deltaTime = 1 / 5.f; }

	gameData.camera.offset = {GetScreenWidth()/ 2.0f, GetScreenHeight() / 2.0f};

	ClearBackground({75, 75, 150, 255});

#pragma region camera movement
	if(IsKeyDown(KEY_LEFT)) gameData.camera.target.x -= 7.f * deltaTime;
	if(IsKeyDown(KEY_RIGHT)) gameData.camera.target.x += 7.f * deltaTime;
	if(IsKeyDown(KEY_UP)) gameData.camera.target.y -= 7.f * deltaTime;
	if(IsKeyDown(KEY_DOWN)) gameData.camera.target.y += 7.f * deltaTime;
	if(IsKeyPressed(KEY_PAGE_UP)) gameData.camera.zoom += 10.0f;
	if(IsKeyPressed(KEY_PAGE_DOWN)) gameData.camera.zoom -= 10.0f;

#pragma endregion

	Vector2 worldsPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
	int blockX = (int)floor(worldsPos.x);
	int blockY = (int)floor(worldsPos.y);
	 

	if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		auto b = gameData.gameMap.getBlockSafe(blockX, blockY);

		if(b)
		{
			*b =  {};
		}
	}

	if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
	{
		auto b = gameData.gameMap.getBlockSafe(blockX, blockY);

		if(b)
		{
			b->type = gameData.selected_block.type;
		}
	}
	
	
	BeginMode2D(gameData.camera);

	for (int y = 0; y < gameData.gameMap.h; y++)
		for (int x = 0; x < gameData.gameMap.w; x++)
		{

			auto &b = gameData.gameMap.getBlockUnsafe(x, y);

			if (b.type != Block::air)
			{
				Rectangle textureUV;
				textureUV.width = 32;
				textureUV.height = 32;
				textureUV.x = b.type * 32;
				textureUV.y = 0;

				DrawTexturePro(
					assetManager.textures,
					getTextureAtlas(b.type, 0, 32, 32), //source
					{(float)x, (float)y, 1, 1}, //dest
					{0, 0},// origin (top-left corner)
					0.0f, // rotation
					WHITE // tint
				);

			}


		}

	DrawTexturePro(
		assetManager.frame,
		{0,0, (float)assetManager.frame.width, (float)assetManager.frame.height}, //source
		{(float)blockX, (float)blockY, 1, 1}, //dest
		{0, 0},// origin (top-left corner)
		0.0f, // rotation
		WHITE // tint
	);

	EndMode2D();
	

	return true;
}
void closeGame()
{
    std::ofstream f(RESOURCES_PATH "f.txt");
    f << "\nCLOSED\n";
    f.close();

}