#include "worldGenerator.h"
#include "randomStuff.h"
#include "FastNoiseSIMD.h"
#include <memory> // to use unique_ptr
#include <iostream>
#include <array>

float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

std::vector<Biome> generateBiomes(int worldWidth, int seed) {

	constexpr std::array<BiomeDefinition, 3> biomeDefinitions{
		BiomeDefinition{BiomeType::Plains, 200, 500},
		BiomeDefinition{BiomeType::Mountains, 150, 350},
		BiomeDefinition{BiomeType::Desert, 300, 600}
	};

	// do some rng stuff and generate
	std::vector<Biome> biomes;
	std::ranlux24_base rng(seed);
	
	int biomesWidth = 0;
	
	int i = 0;
	
	std::cout << "Hello\n";

	// TODO: later i dont have to make this fixed, i could use randomChance to have random biomes with different probabilites, plains always the first biome
	// like this different seeds have different biome orders, sizes etc. 
	while(biomesWidth < worldWidth) {
		const auto& biome = biomeDefinitions[i % biomeDefinitions.size()];
		
		Biome newBiome{};
	
		newBiome.type = biome.type;
		newBiome.actualSize = getRandomInt(rng, biome.minSize, biome.maxSize);
		newBiome.startX = biomesWidth;
		
		biomesWidth += newBiome.actualSize;

		if (biomesWidth > worldWidth) {
			std::cout << "Biomes width is bigger: " << biomesWidth << '\n';
			newBiome.actualSize -= biomesWidth - worldWidth;
			biomesWidth = worldWidth;
			std::cout << "Biomes width after trim: " << biomesWidth << '\n';
		}

		biomes.push_back(newBiome);

		i++;
	}
	
	return biomes; 
}

void generateWorld(GameMap &gameMap, int seed, int dirtOctaves, float dirtFrequency, int stoneOctaves, float stoneFrequency)
{
	// TODOS:
	// more stones they can be above dirt
	// plains not plain enough

	const int w = 4000;
	const int h = 500;
	
	gameMap.create(w, h);
	
	const std::vector<Biome> biomes = generateBiomes(w, seed);

	//std::ranlux24_base rng(seed++);


	std::unique_ptr<FastNoiseSIMD> dirtNoiseGenrator(FastNoiseSIMD::NewFastNoiseSIMD());
	std::unique_ptr<FastNoiseSIMD> stoneNoiseGenrator(FastNoiseSIMD::NewFastNoiseSIMD());

    std::unique_ptr<FastNoiseSIMD> plainsNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());
	std::unique_ptr<FastNoiseSIMD> mountainNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());
	std::unique_ptr<FastNoiseSIMD> regionNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());


	dirtNoiseGenrator->SetSeed(seed++);
	stoneNoiseGenrator->SetSeed(seed++);

    plainsNoiseGenerator->SetSeed(seed++);
    mountainNoiseGenerator->SetSeed(seed++);
    regionNoiseGenerator->SetSeed(seed++);


	dirtNoiseGenrator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	dirtNoiseGenrator->SetFractalOctaves(dirtOctaves);
	dirtNoiseGenrator->SetFrequency(dirtFrequency);
    
	stoneNoiseGenrator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	stoneNoiseGenrator->SetFractalOctaves(stoneOctaves);
	stoneNoiseGenrator->SetFrequency(stoneFrequency);


	plainsNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	plainsNoiseGenerator->SetFractalOctaves(2);
	plainsNoiseGenerator->SetFrequency(0.004f);

	mountainNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	mountainNoiseGenerator->SetFractalOctaves(3);
	mountainNoiseGenerator->SetFrequency(0.015f);

	regionNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	regionNoiseGenerator->SetFractalOctaves(1);
	regionNoiseGenerator->SetFrequency(0.0035f);



	float *dirtNoise = FastNoiseSIMD::GetEmptySet(w);
	float *stoneNoise = FastNoiseSIMD::GetEmptySet(w);

    float *plainsNoise = FastNoiseSIMD::GetEmptySet(w);
    float *mountainsNoise = FastNoiseSIMD::GetEmptySet(w);
    float *regionNoise = FastNoiseSIMD::GetEmptySet(w);
    
	dirtNoiseGenrator->FillNoiseSet(dirtNoise, 0, 0, 0, w, 1, 1);
	stoneNoiseGenrator->FillNoiseSet(stoneNoise, 0, 0, 0, w, 1, 1);

	plainsNoiseGenerator->FillNoiseSet(plainsNoise, 0, 0, 0, w, 1, 1);
	mountainNoiseGenerator->FillNoiseSet(mountainsNoise, 0, 0, 0, w, 1, 1);
	regionNoiseGenerator->FillNoiseSet(regionNoise, 0, 0, 0, w, 1, 1);


	int regionCounter = 0;
	//convert from [-1 1] to [0 1]
	for (int i = 0; i < w; i++)
	{
		dirtNoise[i] = (dirtNoise[i] + 1) / 2;
		stoneNoise[i] = (stoneNoise[i] + 1) / 2;
		plainsNoise[i] = (plainsNoise[i] + 1) / 2;
		mountainsNoise[i] = (mountainsNoise[i] + 1) / 2;
		regionNoise[i] = (regionNoise[i] + 1) / 2;

		//stoneNoise[i] = std::pow(stoneNoise[i], 2); //steeper mountains.
	}

	int dirtThicknessMin = -5;
	int dirtThicknessMax = 35;
	
	int plainsHeightStart = 200;
	int plainsHeightEnd = 225;

	int mountainsHeightStart = 100;
	int mountainsHeightEnd = 200;


	for (int x = 0; x < w; x++)
	{

		float plainsHeight = lerp(
				plainsHeightStart,
				plainsHeightEnd,
				plainsNoise[x]
			);

		float mountainsHeight = lerp(
				mountainsHeightStart,
				mountainsHeightEnd,
				mountainsNoise[x]
			);

		float finalHeight = lerp(
			plainsHeight,
			mountainsHeight,
			regionNoise[x]
		);
		
		
		if (regionNoise[x] > 0.5f)
    		regionCounter++;
		//std::cout << regionNoise[x] << '\n';

		int dirtThickness = dirtThicknessMin + (dirtThicknessMax - dirtThicknessMin) * dirtNoise[x];

		int surfaceY = finalHeight;
		int stoneStartY = surfaceY + dirtThickness;

		for (int y = 0; y < h; y++)
		{
			Block b{};

			if (y > surfaceY)
			{
				b.type = Block::dirt;
			}

			if (y == surfaceY)
			{
				b.type = Block::grassBlock;
			}

			if (y >= stoneStartY)
			{
				b.type = Block::stone;
			}

			gameMap.getBlockUnsafe(x, y) = b;


		}

		

	}

	std::cout << "region counter: " << regionCounter << "\n";

	FastNoiseSIMD::FreeNoiseSet(dirtNoise);
	FastNoiseSIMD::FreeNoiseSet(stoneNoise);
    FastNoiseSIMD::FreeNoiseSet(plainsNoise);
    FastNoiseSIMD::FreeNoiseSet(mountainsNoise);
    FastNoiseSIMD::FreeNoiseSet(regionNoise);
}