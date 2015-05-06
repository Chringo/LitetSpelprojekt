#ifndef TILE_H
#define TILE_H

class Tile
{
public:
	Tile();
	Tile(float startValue);
	~Tile();

	void setObstacle(bool obstacle);
	bool getObstacle() const;

	void setHeight(float height);
	float getHeight() const;
private:
	bool obstacle;

	float height;

	//TODO worldposition
	//TODO type of tile
};
#endif