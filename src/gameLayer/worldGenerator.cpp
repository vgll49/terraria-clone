#include "worldGenerator.h"
#include "randomStuff.h"
#include <FastNoiseSIMD.h>
#include <memory>
#include <iostream>
#include <algorithm>


float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

float screenBlend(float base, float blend)
{
    return 1.0f - (1.0f - base) * (1.0f - blend);
}

void generateWorld(GameMap &gameMap, int seed)
{
	const int w = 900;
	const int h = 500;

	gameMap.create(w, h);
	

	std::ranlux24_base rng(seed++);

	int desertStart = getRandomInt(rng, 10, w - 210);
	int desertEnd = desertStart + 100 + getRandomInt(rng, 0, 100);
	if (desertEnd > w) { desertEnd = w; }


	std::unique_ptr<FastNoiseSIMD> dirtNoiseGenrator(FastNoiseSIMD::NewFastNoiseSIMD());
	std::unique_ptr<FastNoiseSIMD> simplexCavesNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());
	std::unique_ptr<FastNoiseSIMD> cellularCavesNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());
	std::unique_ptr<FastNoiseSIMD> cavesTypeNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());

	dirtNoiseGenrator->SetSeed(seed++);
	simplexCavesNoiseGenerator->SetSeed(seed++);
	cellularCavesNoiseGenerator->SetSeed(seed++);

	dirtNoiseGenrator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	dirtNoiseGenrator->SetFractalOctaves(6);
	dirtNoiseGenrator->SetFractalGain(0.4f); // lower gain = sharper
	dirtNoiseGenrator->SetFrequency(0.01f);
	
	simplexCavesNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	simplexCavesNoiseGenerator->SetFractalOctaves(3);
	simplexCavesNoiseGenerator->SetFrequency(0.02f);
	
	cellularCavesNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::Cellular);
	cellularCavesNoiseGenerator->SetFrequency(0.1f);

	cellularCavesNoiseGenerator->SetCellularDistanceFunction(
		FastNoiseSIMD::CellularDistanceFunction::Euclidean
	);

	cellularCavesNoiseGenerator->SetCellularReturnType(
		FastNoiseSIMD::CellularReturnType::CellValue
	);
		
	cavesTypeNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	cavesTypeNoiseGenerator->SetFractalOctaves(3);
	cavesTypeNoiseGenerator->SetFrequency(0.05f);

	float *dirtNoise = FastNoiseSIMD::GetEmptySet(w);
	
	dirtNoiseGenrator->FillNoiseSet(dirtNoise, 0, 0, 0, w, 1, 1);

	//convert from [-1 1] to [0 1]
	for (int i = 0; i < w; i++)
	{
		dirtNoise[i] = (dirtNoise[i] + 1) / 2;
	}

	float* simplexCavesNoise = FastNoiseSIMD::GetEmptySet(w * h);
	simplexCavesNoiseGenerator->FillNoiseSet(simplexCavesNoise, 0, 0, 0, h, w, 1); // flipped for some reason
	
	float* cellularCavesNoise = FastNoiseSIMD::GetEmptySet(w * h);
	cellularCavesNoiseGenerator->FillNoiseSet(cellularCavesNoise, 0, 0, 0, h, w, 1); // flipped for some reason

	float* cavesTypeNoise = FastNoiseSIMD::GetEmptySet(w * h);
	cavesTypeNoiseGenerator->FillNoiseSet(cavesTypeNoise, 0, 0, 0, h, w, 1); // flipped for some reason

	//convert from [-1 1] to [0 1]
	for (int i = 0; i < w * h; i++)
	{
		simplexCavesNoise[i] = (simplexCavesNoise[i] + 1) / 2;		
		cellularCavesNoise[i] = (cellularCavesNoise[i] + 1) / 2;
		cavesTypeNoise[i] = (cavesTypeNoise[i] + 1) / 2;
	}

	// lambda
	auto getNoiseXY = [&](int x, int y, float set[])
	{
		return set[x + y * w];
	};


	auto perlinWorms = [&]() 
	{
		std::cout << "wormin..\n";
		int wormCount = 20; // maybe some value that makes sense later, based on map size 


		// size of the worm has to vary (just a few blocks are ok)

		// dir of the worm has to vary (doesnt matter)

		for(int j = 0; j < wormCount; j++) 
		{

			float startX = getRandomInt(rng, 0, w);
			float startY = getRandomInt(rng, 90, h);
	
			float radius = ((
				getRandomFloat(rng, 2, 5) +
				getRandomFloat(rng, 2, 5) +
				getRandomFloat(rng, 2, 7)
			) / 3);
	
			float dirX = getRandomFloat(rng, -1, 1);
			float dirY = getRandomFloat(rng, -1, 1);
		
			int dirTime = getRandomInt(rng, 5, 40);
		
			int wormLength = 
			((
				getRandomInt(rng, 50, 100) +
				getRandomInt(rng, 100, 500) +
				getRandomInt(rng, 200, 700)
			) / 3);

			std::cout << "Length: " << wormLength << '\n';
			
			
			float x = startX;
			float y = startY;

			for(int i = 0; i<wormLength; i++) {
				
				// diggin a circle around current pos
				int intRadius = std::ceil(radius);

				for(int offX = -intRadius; offX <= intRadius; offX++)
				{
					for(int offY = -intRadius; offY <= intRadius; offY++)
					{
						float distSq = offX * offX + offY * offY;

						if(distSq <= radius*radius)
						{
							int digX = x + offX;
							int digY = y + offY;

							auto b = gameMap.getBlockSafe(digX, digY);
							if (b)
							{
								b->type = Block::air;
							}
						}

					}
				}
				
				dirTime--;
				if (dirTime <= 0)
				{
					dirTime = getRandomInt(rng, 5, 20);

					if(getRandomChance(rng, 0.7))
					{
						float keepFactor = 0.8;
						// big chance we keep a very similar direction
						dirX = dirX * keepFactor + (getRandomFloat(rng, -1, 1)) * (1.f - keepFactor);
						dirY = dirY * keepFactor + (getRandomFloat(rng, -1, 1)) * (1.f - keepFactor);
					} 
					else
					{
						float keepFactor = 0.2;
						// small chance we keep a very similar direction
						dirX = dirX * keepFactor + (getRandomFloat(rng, -1, 1)) * (1.f - keepFactor);
						dirY = dirY * keepFactor + (getRandomFloat(rng, -1, 1)) * (1.f - keepFactor);
					}
				}

				// Move forward
				x += dirX * 1.5f;
				y += dirY * 1.5f;
				
				// Random radius wobble
				radius += (getRandomFloat(rng, -0.2, 0.2));
				radius = std::clamp(radius, 2.2f, 8.5f);
			}
			// set block? 
		}

	};

	/*
	random start point

	
	lenght
	
	direction (that changes traversing)
	
	size 

	scatter around map (maybe spawn based on seed)
	
	*/



	int dirtOffsetStart = -5;
	int dirtOffsetEnd = 35;


	int keepDirectionTimeStone = getRandomInt(rng, 5, 40);
	int directionStone = getRandomInt(rng, -2, 2);


	int stoneHeight = 90;

	for (int x = 0; x < w; x++)
	{

		bool inDesert = (x >= desertStart && x <= desertEnd);


	#pragma region stone height


		keepDirectionTimeStone--;
		if (keepDirectionTimeStone <= 0)
		{
			keepDirectionTimeStone = getRandomInt(rng, 5, 40);
			directionStone = getRandomInt(rng, -2, 2);
		}

		if (directionStone == -1)
		{
			if (getRandomChance(rng, 0.25))
			{
				stoneHeight--;
			}
		}
		else if (directionStone == -2)
		{
			if (getRandomChance(rng, 0.25))
			{
				stoneHeight--;
			}

			if (getRandomChance(rng, 0.25))
			{
				stoneHeight--;
			}
		}
		else if (directionStone == 1)
		{
			if (getRandomChance(rng, 0.25))
			{
				stoneHeight++;
			}
		}
		else if (directionStone == 2)
		{
			if (getRandomChance(rng, 0.25))
			{
				stoneHeight++;
			}

			if (getRandomChance(rng, 0.25))
			{
				stoneHeight++;
			}
		}

		if (stoneHeight < 60)
		{
			stoneHeight = 60;
		}

		if (stoneHeight > 120)
		{
			stoneHeight = 120;
		}

	#pragma endregion


		int dirtHeight = dirtOffsetStart + (dirtOffsetEnd - dirtOffsetStart) * dirtNoise[x];
		dirtHeight = stoneHeight - dirtHeight;


		int dirtType = Block::dirt;
		int grassType = Block::grassBlock;
		int stoneType = Block::stone;

		if (inDesert)
		{
			dirtType = Block::sand;
			grassType = Block::sand;
			stoneType = Block::sandStone;
		}

		for (int y = 0; y < h; y++)
		{
			Block b;

			if (y > dirtHeight)
			{
				b.type = dirtType;
			}

			if (y == dirtHeight)
			{
				b.type = grassType;
			}

			if (y >= stoneHeight)
			{
				b.type = stoneType;
			}

			if (inDesert)
			{
				int desertMid = (desertStart + desertEnd) / 2;
				int desertHalfWidth = (desertEnd - desertStart) / 2;
				int distanceFromDesertMid = std::abs(x - desertMid);

				// This gives a value from 0 at edge to 1 at center
				float desertDistance = 1 - distanceFromDesertMid / float(desertHalfWidth);

				int desertStoneStart = 10 + stoneHeight;
				int desertStoneDepth = 20 + stoneHeight; // how deep the triangle goes

				int triangleStoneY = desertStoneStart + desertDistance * desertStoneDepth;

				// Apply stone if below the triangle
				if (y > triangleStoneY)
				{
					b.type = Block::stone;
				}
			}

			// if(getNoiseXY(x, y, simplexCavesNoise) < 0.3) 
			// {
			// 	b.type = Block::air;
			// }

			// if(lerp(getNoiseXY(x, y, simplexCavesNoise), getNoiseXY(x, y, cellularCavesNoise), getNoiseXY(x, y, cavesTypeNoise)) < 0.2) 
			// {
			// 	b.type = Block::air;
			// }
			
			gameMap.getBlockUnsafe(x, y) = b;

		}

	}
	perlinWorms();

	FastNoiseSIMD::FreeNoiseSet(dirtNoise);
	FastNoiseSIMD::FreeNoiseSet(simplexCavesNoise);
	FastNoiseSIMD::FreeNoiseSet(cellularCavesNoise);
	FastNoiseSIMD::FreeNoiseSet(cavesTypeNoise);

}
