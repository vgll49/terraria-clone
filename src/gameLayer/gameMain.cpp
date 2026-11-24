#include <raylib.h>
#include <raymath.h>
#include "gameMain.h"
#include <asserts.h>
#include <assetManager.h>
#include <gameMap.h>
#include <helpers.h>
#include <randomStuff.h>
#include <iostream>
#include <fstream>
#include <imgui.h>
#include <worldGenerator.h>

struct GameData 
{
    GameMap gameMap;
	Camera2D camera;

	int creativeSelectedBlock; 

}gameData;

AssetManager assetManager;

bool showImgui = false; 

bool initGame()
{

    assetManager.loadAll();

	generateWorld(gameData.gameMap);
    	
	// Camera
	gameData.camera.target = {20, 120}; // world-space center of view
	gameData.camera.rotation = 0.0f;
	gameData.camera.zoom = 10.0f;

    return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	if (deltaTime > 1.f / 5) { deltaTime = 1 / 5.f; }

	gameData.camera.offset = {GetScreenWidth()/ 2.0f, GetScreenHeight() / 2.0f};

	ClearBackground({75, 75, 150, 255});

	if(IsKeyPressed(KEY_F10)) showImgui = !showImgui;


	static float CAMERA_SPEED = 100;

#pragma region camera movement
	if(IsKeyDown(KEY_LEFT)) gameData.camera.target.x -= CAMERA_SPEED * deltaTime;
	if(IsKeyDown(KEY_RIGHT)) gameData.camera.target.x += CAMERA_SPEED * deltaTime;
	if(IsKeyDown(KEY_UP)) gameData.camera.target.y -= CAMERA_SPEED * deltaTime;
	if(IsKeyDown(KEY_DOWN)) gameData.camera.target.y += CAMERA_SPEED * deltaTime;
	if(IsKeyPressed(KEY_PAGE_UP)) gameData.camera.zoom += 10.0f;
	if(IsKeyPressed(KEY_PAGE_DOWN)) gameData.camera.zoom -= 10.0f;

#pragma endregion

	Vector2 worldsPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
	int blockX = (int)floor(worldsPos.x);
	int blockY = (int)floor(worldsPos.y);
	 
	if(gameData.creativeSelectedBlock < 0) gameData.creativeSelectedBlock = 0;
	if(gameData.creativeSelectedBlock >= Block::BLOCKS_COUNT) gameData.creativeSelectedBlock = Block::BLOCKS_COUNT -1;

	if(!showImgui) 
	{

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			// TODO: later adapt this to some kind of mining system. So that IsMouseButtonDown and the layer does not change or some other way
			if (auto* b = gameData.gameMap.getBlockSafe(blockX, blockY);
				b && b->type != Block::air)
			{
				*b = {};
			}
			else if (auto* b = gameData.gameMap.getBlockSafe(
						blockX, blockY, MapLayer::Background);
					b && b->type != Block::air)
			{
				*b = {};
			}

			std::cout << "Block X: " << blockX
			<< " | Block Y: " << blockY << '\n';
		}

		if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		{
			uint64_t seed = hashCoordinates(blockX, blockY);
			std::ranlux24_base rng(seed);
			int blockVariation = getRandomInt(rng, 0, 3);
			
			auto* b = gameData.gameMap.getBlockSafe(blockX, blockY);

			if (isWallBlock(gameData.creativeSelectedBlock))
				b = gameData.gameMap.getBlockSafe(blockX, blockY, MapLayer::Background);

			if (b)
			{
				b->type = gameData.creativeSelectedBlock;
				b->variation = blockVariation;
			}
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
			auto &bB = gameData.gameMap.getBlockUnsafe(x, y, MapLayer::Background);

			if(bB.type != Block::air) {
				DrawTexturePro(
						assetManager.textures,
						getTextureAtlas(bB.type, bB.variation, 32, 32), //source
						{(float)x, (float)y, 1, 1}, //dest
						{0, 0},// origin (top-left corner)
						0.0f, // rotation
						WHITE // tint
					);
			}

			if (b.type != Block::air)
			{
				if(b.type == Block::woodLog)  
				{
					Rectangle atlas_source = getTextureAtlas(0, bB.variation, 32, 32);
					// 1. get blocks around
					Block *bLeft = gameData.gameMap.getBlockSafe(x-1, y);
					Block *bRight = gameData.gameMap.getBlockSafe(x+1, y);
					Block *bTop= gameData.gameMap.getBlockSafe(x, y-1);
					Block *bBottom = gameData.gameMap.getBlockSafe(x, y+1);

					// handle the cases and render block based on this
					
					if(bTop->type == Block::woodLog && bBottom->type != Block::woodLog) {
						atlas_source = getTextureAtlas(4, b.variation, 32, 32);
						
					}

					if(bLeft->type == Block::leaves) {
						atlas_source = getTextureAtlas(3, b.variation, 32, 32);
						
					} 

					if(bRight->type == Block::leaves) {
						atlas_source = getTextureAtlas(2, b.variation, 32, 32);
					}

					if(bRight->type == Block::leaves && bLeft->type == Block::leaves) {
						atlas_source = getTextureAtlas(1, b.variation, 32, 32);
					}
					
					if(bTop->type == Block::leaves && bBottom->type != Block::leaves) {
						atlas_source = getTextureAtlas(5, b.variation, 32, 32);
					}
					
					if(bTop->type != Block::leaves && bTop->type != Block::woodLog &&  bRight->type != Block::leaves && bLeft->type != Block::leaves && bBottom->type == Block::woodLog) {
						atlas_source = getTextureAtlas(6, b.variation, 32, 32);
					}

					if(bTop->type != Block::leaves && bTop->type != Block::woodLog && bBottom->type != Block::woodLog && bBottom->type != Block::leaves) {
						atlas_source = getTextureAtlas(7, b.variation, 32, 32);
					}

					DrawTexturePro(
						assetManager.tree_textures,
						atlas_source, //source
						{(float)x, (float)y, 1, 1}, //dest
						{0, 0},// origin (top-left corner)
						0.0f, // rotation
						WHITE // tint
					);

				} else {			
					DrawTexturePro(
						assetManager.textures,
						getTextureAtlas(b.type, b.variation, 32, 32), //source
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

	if(showImgui) 
		{

		ImGui::Begin("Debug Stuff");
					
		for(int i = 0; i<Block::BLOCKS_COUNT; i++) 
		{
			auto atlas = getTextureAtlas(i, 0, 32, 32);
			atlas.x /= assetManager.textures.width;
			atlas.width /= assetManager.textures.width;
			atlas.y /= assetManager.textures.height;
			atlas.height /= assetManager.textures.height;

			ImGui::PushID(i);

			ImTextureID tex = (ImTextureID)(intptr_t)assetManager.textures.id;
			if(ImGui::ImageButton(
				tex, {35,35}, {atlas.x, atlas.y}, {atlas.x + atlas.width, atlas.y+atlas.height} 	
				))
			{
				gameData.creativeSelectedBlock = i;
			}

			ImGui::PopID();

			if(i % 10 != 0)
			{
				ImGui::SameLine();
			}
		}

		ImGui::NewLine();
		ImGui::SliderFloat("Camera zoom:", &gameData.camera.zoom, 10, 150);
		ImGui::SliderFloat("Camera speed:", &CAMERA_SPEED, 5, 100);

		ImGui::End();

		}
	
	DrawFPS(10, 10);

	return true;
}



void closeGame()
{
    std::ofstream f(RESOURCES_PATH "f.txt");
    f << "\nCLOSED\n";
    f.close();

}