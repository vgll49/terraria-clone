#include "worldGenerator.h"
#include "randomStuff.h"
#include <FastNoiseSIMD.h>
#include <memory>
#include <iostream>
#include <algorithm>

/*
TODOS:
1. Tweak noises for caves (maybe use cellular etc. again and combine with noise that chooses between)
2. Implement function for different perlinWorm usages, maybe based on biome/surface
3. rm debug stuff
*/

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
	
	float *dirtNoise = FastNoiseSIMD::GetEmptySet(w);
	
	dirtNoiseGenrator->FillNoiseSet(dirtNoise, 0, 0, 0, w, 1, 1);

	//convert from [-1 1] to [0 1]
	for (int i = 0; i < w; i++)
	{
		dirtNoise[i] = (dirtNoise[i] + 1) / 2;
	}

	float* simplexCavesNoise = FastNoiseSIMD::GetEmptySet(w * h);
	simplexCavesNoiseGenerator->FillNoiseSet(simplexCavesNoise, 0, 0, 0, h, w, 1); // flipped for some reason

	//convert from [-1 1] to [0 1]
	for (int i = 0; i < w * h; i++)
	{
		simplexCavesNoise[i] = (simplexCavesNoise[i] + 1) / 2;		
	}

	// lambda
	auto getNoiseXY = [&](int x, int y, float set[])
	{
		return set[x + y * w];
	};

	int wormCount = 0;
	auto spawnPerlinWorm = [&](auto&& self, float startX, float startY, float maxLength, float maxR) -> void
	{
		int maxLen = static_cast<int>(maxLength);

		if (maxLen < 10 || maxR < 1.0f)
			return;

		float minRadius = std::min(2.2f, maxR * 0.6f);
		float radius = getRandomFloat(rng, minRadius, maxR);

		float dirX = getRandomFloat(rng, -1, 1);
		float dirY = getRandomFloat(rng, -1, 1);

		int dirTime = getRandomInt(rng, 5, 40);

		int wormLength = (
			getRandomInt(rng, std::max(1, static_cast<int>(maxLen * 0.07f)),
							static_cast<int>(maxLen * 0.4f)) +

			getRandomInt(rng, std::max(1, static_cast<int>(maxLen * 0.14f)),
							static_cast<int>(maxLen * 0.8f)) +

			getRandomInt(rng, std::max(1, static_cast<int>(maxLen * 0.28f)),
							maxLen)
		) / 3;

		std::cout << "Length: " << wormLength << '\n';

		int childSpawnAt = -1;

		if (getRandomChance(rng, 0.1)) {
			childSpawnAt = getRandomInt(rng, 0, wormLength - 1);
		}
		
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
			radius = std::clamp(radius, minRadius, maxR);

			// 10 percent chance to spawn tinier worm
			if (i == childSpawnAt) {
				self(self, x, y, maxLength / 2, maxR / 2);
			}
		}
	};

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

			if(getNoiseXY(x, y, simplexCavesNoise) < 0.3) 
			{
				b.type = Block::air;
			}
		
			gameMap.getBlockUnsafe(x, y) = b;

		}

	}

	int count = 0;
	for (int i = 0; i < w; i += 200)
	{
		for (int j = 0; j < h; j += 150)
		{
			count++;
			float startX = i + getRandomFloat(rng, 0, 200/2);
			float startY = j + getRandomFloat(rng, 0, 150/2);

			startX = std::min(startX, static_cast<float>(w - 1));
			startY = std::min(startY, static_cast<float>(h - 1));

			spawnPerlinWorm(
				spawnPerlinWorm, startX, startY, 700, 7.5f
			);
		}
	}
	std::cout << "blue noise worms: " << count << '\n';

	FastNoiseSIMD::FreeNoiseSet(dirtNoise);
	FastNoiseSIMD::FreeNoiseSet(simplexCavesNoise);

	std::cout << "WORM COUNT: " << wormCount << '\n';

}
