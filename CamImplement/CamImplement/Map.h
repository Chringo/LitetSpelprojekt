#ifndef MAP_H
#define MAP_H
#define TILESIZE 2.0f

#include <d3d11.h>
#include <DirectXMath.h>
//#include <math.h>
//#include <random>
#include "TileClass.h"

class Map
{	
private:
	TileClass** tiles;
	int chunkSize;
	float seed;
	float random;
	void CreateTiles();
	
	// Diamond-Square
	float** ds;
	float offset;
	void DiamondSquare(float range, float decrease);

	// Evaluation
	int water;
	int hill;
	void EvaluateTile(TileClass tile);
	bool EvaluateMap();

public:
	Map();
	Map(int exponent, float startValue);//TODO Map starting point others than origo
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

	void setTileMatrices();
	DirectX::XMMATRIX* getTileMatrices() const;
	
	int pow(int base, int exponent);
};
#endif