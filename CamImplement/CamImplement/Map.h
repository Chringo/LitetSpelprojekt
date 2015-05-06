#ifndef MAP_H
#define MAP_H

#include <d3d11.h>
//#include <math.h>
//#include <random>
#include "Tile.h"

class Map
{
public:
	Map();
	Map(int exponent, float startValue);
	Map(int exponent, float startValue, int seed);
	~Map();

	void setSeed(float seed);
	float getSeed() const;
	void setChunkSize(int size);
	int getChunkSize() const;
	void setOffset(float offset);
	float getOffset() const;
	void setRandom(int value);
	float getRandom();
	
	int pow(int base, int exponent);
	
private:
	Tile** tiles;
	int chunkSize;
	float seed;
	float random;
	void LoadTiles();

	// Diamond-Square
	float** ds;
	float offset;
	void DiamondSquare(float range, float decrease);

	// Evaluation
	int water;
	int hill;
	void EvaluateTile(Tile tile);
	bool EvaluateMap();
};
#endif