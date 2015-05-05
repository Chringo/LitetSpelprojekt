#ifndef MAP_H
#define MAP_H

#include <d3d11.h>
#include <math.h>
#include "Tile.h"

class Map
{
public:
	Map();
	Map(int power, float startValue);
	Map(int power, float startValue, int seed);
	~Map();

	void setSeed(float seed);
	float getSeed() const;
	void setChunkSize(int size);
	int getChunkSize() const;
	void setOffset(float offset);
	float getOffset() const;
	
private:
	Tile** tiles;
	float seed;
	int chunkSize;
	//TODO Random
	void loadTiles();

	// Diamond-Square
	float** ds;
	float off;
	void DiamondSquare(float range, float decrease);

	// Evaluation
	int water;
	int hill;
	void evaluateTile(Tile tile);
	bool evaluateMap();
};
#endif