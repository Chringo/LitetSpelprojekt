#ifndef MAP_H
#define MAP_H

#include <d3d11.h>
#include <DirectXMath.h>
#include "TileClass.h"

class Map
{	
private:
	TileClass** tiles;
	BaseTile** baseTiles;
	int chunkSize;
	float seed;
	float random;
	void CreateTiles();
	
	// Diamond-Square
	float** ds;
	float disp;// aggressiveness of the displacement
	void DiamondSquare(float range, float decrease);

	// Evaluation
	int water;
	int peak;// might not be needed
	void EvaluateTile(TileClass& tile);//TODO expand the variations
	bool EvaluateMap();//TODO more accurate eval

	// Calcs
	int pow(int base, int exponent);
	float averageOfSquare(int height, int width, int index);

public:
	const float TILESIZE = 4.0f;

	Map();
	Map(int randSeed, int exponent, float startValue);//TODO Map starting point others than origo
	virtual ~Map();

	void setSeed(float seed);
	float getSeed() const;
	void setChunkSize(int size);
	int getChunkSize() const;
	void setOffset(float offset);
	float getOffset() const;
	void setRandom(int value);
	float getRandom();

	int getWater() const;
	int getPeak() const;
	int getObstacles() const;

	BaseTile** getBaseTiles();

	// Graphics data
	void setTileMatrices();
	DirectX::XMMATRIX setMapPlane() const;
	int getNrOfTiles() const;
};
#endif