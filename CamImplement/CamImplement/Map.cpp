#include "Map.h"

Map::Map()
{

}
Map::Map(int power, float startValue)
{
	// Initiate point-map
	this->chunkSize = /*(pow(2, power))*/ + 1;// +1 gives the map a mid-point
	seed = startValue;
	ds = new float*[chunkSize, (chunkSize - 1) + 1];// 33x33, 17x17, etc
	chunkSize--;
	// Initiate map
	tiles = new Tile*[chunkSize, chunkSize];// 32x32, 16x16, etc

	int a = rand() % 100;
	// Create noise - algorithm usage
	DiamondSquare(30.0f, 0.76789f);
}
Map::~Map()
{
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
	this->off = offset;
}
float Map::getOffset() const
{
	return this->off;
}

void Map::loadTiles()
{
	float avg;
	for (int h = 0; h < chunkSize; h++)
	{
		for (int w = 0; w < chunkSize; w++)
		{
			avg = ds[h][w] +
				ds[h + 1][w] +
				ds[h][w + 1] +
				ds[h + 1][w + 1];
			avg /= 4;

			tiles[h][w] = Tile(avg);

			evaluateTile(tiles[h][w]);
		}
	}
}
//TODO
void Map::DiamondSquare(float range, float decrease)
{
	// Starting value for corners
	float startOffset = 1.2f;
	ds[0][0] = seed /*+ (Seed * Rand * startOffset)*/;// Top left
	ds[0][chunkSize] = seed /*+ (Seed * Rand * startOffset)*/;// Top right
	ds[chunkSize][0] = seed /*+ (Seed * Rand * startOffset)*/;// Bot left
	ds[chunkSize][chunkSize] = seed /*+ (Seed * Rand * startOffset)*/;// Bot right

	off = range;// the range (-off -> +off) for the average offset
	float avg;
	for (int i = chunkSize; i >= 2; i /= 2, off *= decrease)// decrease the variation of the offset
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
				ds[h + halfI][w + halfI] = avg /*+ (Rand * 2 * off)*/ - off;
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
				avg = avg /*+ (Rand * 2 * off)*/ - off;
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
void Map::evaluateTile(Tile tile)
{
	if (tile.getHeight() < 60)
	{
		tile.setObstacle(true);
		water++;
	}
	//else if (tile.getHeight() > 180)
	//{
	//	tile.setObstacle(true);
	//	hill++;
	//}
}
bool Map::evaluateMap()
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
