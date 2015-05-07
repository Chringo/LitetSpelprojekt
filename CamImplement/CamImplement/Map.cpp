#include "Map.h"

Map::Map()
{
	// Initiate point-map
	this->chunkSize = pow(2, 3) + 1;// +1 gives the map a mid-point
	seed = 60.0f;
	ds = new float*[chunkSize, chunkSize];// 33x33, 17x17, etc
	chunkSize--;
	// Initiate map
	tiles = new TileClass*[chunkSize, chunkSize];// 32x32, 16x16, etc

	// Create noise - algorithm usage
	DiamondSquare(30.0f, 0.76789f);

	// Create Tile-based map
	CreateTiles();
}
Map::Map(int exponent, float startValue)
{
	// Initiate point-map
	this->chunkSize = pow(2, exponent) + 1;// +1 gives the map a mid-point
	seed = startValue;
	ds = new float*[chunkSize, chunkSize];// 33x33, 17x17, etc
	chunkSize--;
	// Initiate map
	tiles = new TileClass*[chunkSize, chunkSize];// 32x32, 16x16, etc

	// Create noise - algorithm usage
	DiamondSquare(30.0f, 0.76789f);

	// Create Tile-based map
	CreateTiles();
}
Map::~Map()
{
	for (int h = 0; h < chunkSize; h++)
	{
		delete tiles[h];
		delete ds[h];
	}
	delete ds[chunkSize + 1];
	delete[] tiles;
	delete[] ds;
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
void Map::setOffset(float offset)
{
	this->offset = offset;
}
float Map::getOffset() const
{
	return this->offset;
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

int Map::pow(int base, int exponent)
{
	int b = base;
	for (int i = 0; i < exponent; i++)
	{
		b *= b;
	}
	return b;
}

void Map::CreateTiles()
{
	float avg;

	// Set world position
	float posX = 0.0f;
	float posZ = 0.0f;
	DirectX::XMFLOAT3 worldpos = { posX, 0, posZ };
	for (int h = 0; h < chunkSize; h++)
	{
		for (int w = 0; w < chunkSize; w++)
		{
			avg = ds[h][w] +
				ds[h + 1][w] +
				ds[h][w + 1] +
				ds[h + 1][w + 1];
			avg /= 4;

			tiles[h][w] = TileClass(avg, worldpos);

			EvaluateTile(tiles[h][w]);

			//posX += TILESIZE;
			//posZ += TILESIZE;
			//worldpos = { posX, 0, posZ };
			worldpos.x += TILESIZE;
			worldpos.z += TILESIZE;
		}
	}
}
DirectX::XMMATRIX* Map::getTileMatrices() const
{
	DirectX::XMMATRIX* arrOfTiles = new DirectX::XMMATRIX[chunkSize * chunkSize];
	for (int h = 0; h < chunkSize; h++)
	{
		for (int w = 0; w < chunkSize; w++)
		{
			*arrOfTiles = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&tiles[h][w].getWorldPos()));
		}
	}
	return arrOfTiles;
}
//TODO
void Map::DiamondSquare(float range, float decrease)
{
	// Starting value for corners
	float startOffset = 1.2f;
	ds[0][0] = seed + (seed * getRandom() * startOffset);// Top left
	ds[0][chunkSize] = seed + (seed * getRandom() * startOffset);// Top right
	ds[chunkSize][0] = seed + (seed * getRandom() * startOffset);// Bot left
	ds[chunkSize][chunkSize] = seed + (seed * getRandom() * startOffset);// Bot right

	offset = range;// the range (-off -> +off) for the average offset
	float avg;
	for (int i = chunkSize; i >= 2; i /= 2, offset *= decrease)// decrease the variation of the offset
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
				ds[h + halfI][w + halfI] = avg + (getRandom() * 2 * offset) - offset;
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
				avg = avg + (getRandom() * 2 * offset) - offset;
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
//TODO
void Map::EvaluateTile(TileClass tile)
{
	if (tile.getHeight() < 60)
	{
		tile.setObstacle(true);
		water++;
	}
	// Define tile type

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
