#ifndef MAP_H
#define MAP_H
#define TILESIZE 4.0f

#include <d3d11.h>
#include <DirectXMath.h>
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
	int hill;// might not needed
	void EvaluateTile(TileClass tile);//TODO expand the variations
	bool EvaluateMap();//TODO more accurate eval

	// Calcs
	int pow(int base, int exponent);

	// Graphics data
	DirectX::XMMATRIX* arrOfTiles;

public:
	Map();
	Map(int exponent, float startValue);//TODO Map starting point others than origo
	Map(int exponent, float startValue, int seed);
	virtual ~Map();

	void setSeed(float seed);
	float getSeed() const;
	void setChunkSize(int size);
	int getChunkSize() const;
	void setOffset(float offset);
	float getOffset() const;
	void setRandom(int value);
	float getRandom();

	// Graphics data
	void setTileMatrices();
	DirectX::XMMATRIX* getTileMatrices();
	int getNrOfTiles() const;
};
#endif