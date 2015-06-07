#include "Map.h"

Map::Map()
{
	// Get seed for noise
	setRandom(1);
	// Initiate point-map
	this->chunkSize = pow(2, 5) + 1;// +1 gives the map a mid-point
	seed = 60.0f;
	ds = new float*[chunkSize];// 33x33, 17x17, etc
	for (int i = 0; i < chunkSize; i++)
	{
		ds[i] = new float[chunkSize];
	}
	chunkSize--;
	// Initiate map
	tiles = new TileClass*[chunkSize];// 32x32, 16x16, etc
	baseTiles = new BaseTile*[chunkSize];
	for (int i = 0; i < chunkSize; i++)
	{
		tiles[i] = new TileClass[chunkSize];
		baseTiles[i] = new BaseTile[chunkSize];
	}

	// Create noise - algorithm usage
	DiamondSquare(30.0f, 0.76789f);

	// Create Tile-based map
	CreateTiles();
}
Map::Map(int randSeed, int exponent, float startValue)
{
	// Get seed for noise
	setRandom(randSeed);
	// Initiate point-map
	this->chunkSize = pow(2, exponent) + 1;// +1 gives the map a mid-point
	seed = startValue;
	ds = new float*[chunkSize];// 33x33, 17x17, etc
	for (int i = 0; i < chunkSize; i++)
	{
		ds[i] = new float[chunkSize];
	}
	chunkSize--;
	// Initiate map
	tiles = new TileClass*[chunkSize];// 32x32, 16x16, etc
	baseTiles = new BaseTile*[chunkSize];
	for (int i = 0; i < chunkSize; i++)
	{
		tiles[i] = new TileClass[chunkSize];
		baseTiles[i] = new BaseTile[chunkSize];
	}

	// Create noise - algorithm usage
	DiamondSquare((startValue * 0.5f), 0.76789f);

	// Create Tile-based map
	CreateTiles();
}
Map::~Map()
{
	for (int h = chunkSize - 1; h > 0; h--)
	{
		delete[] ds[h];// If fatal crash happened, check this one.
		delete[] tiles[h];
		delete[] baseTiles[h];
	}
	delete ds[0];
	delete[] tiles;
	delete[] ds;
	delete[] baseTiles;
}

void Map::setSeed(float seed)
{
	this->seed = seed;
}
float Map::getSeed() const
{
	return this->seed;
}
void Map::setChunkSize(int size)
{
	this->chunkSize = size;
}
int Map::getChunkSize() const
{
	return this->chunkSize;
}
void Map::setOffset(float disp)
{
	this->disp = disp;
}
float Map::getOffset() const
{
	return this->disp;
}
void Map::setRandom(int value)
{
	srand(value);
}
float Map::getRandom()
{
	random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);// Random value between 0.0f -> 1.0f
	return random;
}
int Map::getWater() const
{
	return this->water;
}
int Map::getPeak() const
{
	return this->peak;
}
int Map::getObstacles() const
{
	return (getWater() + getPeak());
}

int Map::pow(int base, int exponent)
{
	int b = 1;
	if (exponent > 0)
	{
		b = 2;
		for (int i = 1; i < exponent; i++)
		{
			b *= base;
		}
	}
	else
	{
		b = 1;
	}
	return b;
}
float Map::averageOfSquare(int height, int width, int index)
{
	float average =
		// calculate average of existing corners
		ds[height][width] +// top left
		ds[height + index][width] +// top right
		ds[height][width + index] +// bot left
		ds[height + index][width + index];// bot right
	average /= 4.0;

	return average;
}

void Map::CreateTiles()
{
	float avg = 0.0f;
	float posX = 0.0f;
	float posZ = 0.0f;
	DirectX::XMFLOAT3 worldpos = { posX, 0.0f, posZ };

	for (int h = 0; h < chunkSize; h++)
	{
		for (int w = 0; w < chunkSize; w++)
		{
			avg = ds[h][w] +
				ds[h + 1][w] +
				ds[h][w + 1] +
				ds[h + 1][w + 1];
			avg /= 4;

			// Set world position
			worldpos.x = (h * TILESIZE);
			worldpos.z = (w * TILESIZE);
			tiles[h][w] = TileClass(avg, worldpos);

			EvaluateTile(tiles[h][w]);
		}
	}
}
int Map::getNrOfTiles() const
{
	return (chunkSize * chunkSize);
}
DirectX::XMMATRIX Map::setMapPlane() const
{
	//TODO - Should be dynamic
	float center = (chunkSize * 0.5f * TILESIZE) - 2.f;//First point is -2 off origo
	float scale = TILESIZE * chunkSize * 0.25f;
	//DirectX::XMMATRIX mat = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorSet(2.f, 0.1f, 2.f, 1.f)) * (DirectX::XMMatrixScaling(32.f, 1.f, 32.f));
	DirectX::XMMATRIX mat = DirectX::XMMATRIX(
		DirectX::XMVectorSet(scale, 0.f, 0.f, 0.f),
		DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f),
		DirectX::XMVectorSet(0.f, 0.f, scale, 0.f),
		DirectX::XMVectorSet(center, 0.1f, center, 1.f));

	return mat;
}
//TODO Make sure average is okey once visual graphics is at hand
void Map::DiamondSquare(float range, float decrease)
{
	// Starting value for corners
	float startOffset = 1.2f;
	ds[0][0] = seed + (seed * getRandom() * startOffset);// Top left
	ds[0][chunkSize] = seed + (seed * getRandom() * startOffset);// Top right
	ds[chunkSize][0] = seed + (seed * getRandom() * startOffset);// Bot left
	ds[chunkSize][chunkSize] = seed + (seed * getRandom() * startOffset);// Bot right

	disp = range;// the range (-disp -> +disp) for the average displacement
	float avg;
	for (int i = chunkSize; i >= 2; i /= 2, disp *= decrease)// decrease the variation of the offset
	{
		int halfI = i / 2;
		// generate new square values
		for (int h = 0; h < chunkSize; h += i)
		{
			for (int w = 0; w < chunkSize; w += i)
			{
				// calculate average of existing corners
				avg = ds[h][w] +// top left
					ds[h + i][w] +// top right
					ds[h][w + i] +// bot left
					ds[h + i][w + i];// bot right
				avg /= 4.0;

				// center is average plus random offset
				ds[h + halfI][w + halfI] = avg + (getRandom() * 2 * disp) - disp;
			}
		}//__SQUARE_END__//

		// generate the diamond values
		for (int h = 0; h < (chunkSize + 1); h += halfI)
		{
			for (int w = (h + halfI) % i; w < (chunkSize + 1); w += i)
			{
				avg = ds[(h - halfI + chunkSize) % (chunkSize)][w] +// left of center
					ds[(h + halfI) % (chunkSize)][w] +// right of center
					ds[h][(w + halfI) % (chunkSize)] +// below center
					ds[h][(w - halfI + chunkSize) % (chunkSize)];// above center
				avg /= 4.0;

				// new value = average plus random offset
				avg = avg + (getRandom() * 2 * disp) - disp;
				// update value
				ds[h][w] = avg;

				// remove this below and adjust loop condition above
				if (h == 0)
				{
					ds[chunkSize][w] = avg;
				}
				if (w == 0) // Due to modulus-usage
				{
					ds[h][chunkSize] = avg;
				}
			}
		}//__DIAMOND_END__//
	}//__HEIGHT_MAP_END__//
}
//TODO Set tile type
void Map::EvaluateTile(TileClass& tile)
{
	if (tile.getHeight() < 48)
	{
		tile.setObstacle(true);
		tile.setType(1);
		water++;
	}
	else if (tile.getHeight() > 74 && tile.getHeight() < 76)
	{
		tile.setObstacle(true);
		tile.setType(2);
		peak++;
	}
	else if (tile.getHeight() > 82 && tile.getHeight() < 84)
	{
		tile.setObstacle(true);
		tile.setType(2);
		peak++;
	}
	else if (tile.getHeight() > 120 && tile.getHeight() < 125)
	{
		tile.setObstacle(true);
		tile.setType(2);
		peak++;
	}
	else if (tile.getHeight() > 145)
	{
		tile.setObstacle(true);
		tile.setType(2);
		peak++;
	}
	else
	{
		tile.setType(0);
	}
}
bool Map::EvaluateMap()
{
	bool redo = false;
	int mapSize = chunkSize * chunkSize;
	float calc = (float)mapSize;
	calc = water / calc;
	if (calc < 0.03 || calc > 0.18)
	{
		redo = true;
		water = 0;
	}
	return redo;
}

BaseTile** Map::getBaseTiles()
{
	baseTiles = new BaseTile*[chunkSize];
	for (int h = 0; h < chunkSize; h++)
	{
		baseTiles[h] = new BaseTile[chunkSize];
		for (int w = 0; w < chunkSize; w++)
		{
			baseTiles[h][w] = tiles[h][w].getBase();
		}
	}
	return baseTiles;
}