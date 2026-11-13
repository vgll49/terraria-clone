#include <raylib.h>
#include <raymath.h>
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

	gameData.gameMap.create(700, 500);
 
    gameData.gameMap.getBlockUnsafe(0, 0).type = Block::leaves;
    gameData.gameMap.getBlockUnsafe(1, 1).type = Block::leaves;
    gameData.gameMap.getBlockUnsafe(2, 2).type = Block::leaves;
    gameData.gameMap.getBlockUnsafe(3, 3).type = Block::leaves;
    gameData.gameMap.getBlockUnsafe(4, 4).type = Block::leaves;


	gameData.gameMap.getBlockUnsafe(9, 20).type = Block::leaves;
	gameData.gameMap.getBlockUnsafe(10, 20).type = Block::woodLog;
	gameData.gameMap.getBlockUnsafe(11, 20).type = Block::leaves;
	gameData.gameMap.getBlockUnsafe(9, 21).type = Block::leaves;
	gameData.gameMap.getBlockUnsafe(10, 21).type = Block::woodLog;
	gameData.gameMap.getBlockUnsafe(11, 21).type = Block::leaves;
	gameData.gameMap.getBlockUnsafe(9, 22).type = Block::leaves;
	gameData.gameMap.getBlockUnsafe(10, 22).type = Block::woodLog;
	gameData.gameMap.getBlockUnsafe(11, 22).type = Block::leaves;
	gameData.gameMap.getBlockUnsafe(10, 23).type = Block::woodLog;
	gameData.gameMap.getBlockUnsafe(10, 23).type = Block::woodLog;
	gameData.gameMap.getBlockUnsafe(10, 23).type = Block::woodLog;
	gameData.gameMap.getBlockUnsafe(10, 23).type = Block::woodLog;
	gameData.gameMap.getBlockUnsafe(10, 23).type = Block::woodLog;
	gameData.gameMap.getBlockUnsafe(10, 23).type = Block::woodLog;

    
	
	// Camera
	gameData.camera.target = {0,0}; // world-space center of view
	gameData.camera.rotation = 0.0f;
	gameData.camera.zoom = 50.0f;

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
	ImGui::InputInt("Background Blocks", (int *)&gameData.selected_block.type);

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

	Vector2 topLeftView = GetScreenToWorld2D({0,0}, gameData.camera);
	Vector2 bottomRightView = GetScreenToWorld2D({(float)GetScreenWidth(),(float)GetScreenHeight()}, gameData.camera);

	int startXView = (int)floorf(topLeftView.x-1);
	int endXView = (int)ceilf(bottomRightView.x+1);
	int startYView = (int)floorf(topLeftView.y-1);
	int endYView = (int)ceilf(bottomRightView.y+1);

	startXView = Clamp(startXView, 0, gameData.gameMap.w-1);
	endXView = Clamp(endXView, 0, gameData.gameMap.w-1);

	startYView = Clamp(startYView, 0, gameData.gameMap.h-1);
	endYView = Clamp(endYView, 0, gameData.gameMap.h-1);
	
	for (int y = startYView; y < endYView; y++)
		for (int x = startXView; x < endXView; x++)
		{
			

			auto &b = gameData.gameMap.getBlockUnsafe(x, y);

			if (b.type != Block::air)
			{
				if(b.type == Block::woodLog)  
				{
					Rectangle atlas_source = getTextureAtlas(0, 0, 32, 32);
					// 1. get blocks around
					Block *bLeft = gameData.gameMap.getBlockSafe(x-1, y);
					Block *bRight = gameData.gameMap.getBlockSafe(x+1, y);
					Block *bTop= gameData.gameMap.getBlockSafe(x, y-1);
					Block *bBottom = gameData.gameMap.getBlockSafe(x, y+1);

					// handle the cases and render block based on this
					
					if(bTop->type == Block::woodLog && bBottom->type != Block::woodLog) {
						atlas_source = getTextureAtlas(4, 0, 32, 32);
						
					}

					if(bLeft->type == Block::leaves) {
						atlas_source = getTextureAtlas(3, 0, 32, 32);
						
					} 

					if(bRight->type == Block::leaves) {
						atlas_source = getTextureAtlas(2, 0, 32, 32);
					}

					if(bRight->type == Block::leaves && bLeft->type == Block::leaves) {
						atlas_source = getTextureAtlas(1, 0, 32, 32);
					}
					
					if(bTop->type == Block::leaves && bBottom->type != Block::leaves) {
						atlas_source = getTextureAtlas(5, 0, 32, 32);
					}
					
					if(bTop->type != Block::leaves && bTop->type != Block::woodLog &&  bRight->type != Block::leaves && bLeft->type != Block::leaves && bBottom->type == Block::woodLog) {
						atlas_source = getTextureAtlas(6, 0, 32, 32);
					}

					if(bTop->type != Block::leaves && bTop->type != Block::woodLog && bBottom->type != Block::woodLog && bBottom->type != Block::leaves) {
						atlas_source = getTextureAtlas(7, 0, 32, 32);
					}


					DrawTexturePro(
						assetManager.tree_textures,
						atlas_source, //source
						{(float)x, (float)y, 1, 1}, //dest
						{0, 0},// origin (top-left corner)
						0.0f, // rotation
						WHITE // tint
					);
					// later: just add colored leaves to atlas ? then render based on this, move this to other func
					
				
				} else {

			
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
	
	DrawFPS(10, 10);

	return true;
}
void closeGame()
{
    std::ofstream f(RESOURCES_PATH "f.txt");
    f << "\nCLOSED\n";
    f.close();

}