#include "Tile.h"


Tile::Tile()
{
	this->height = 200;
	this->obstacle = false;
}
Tile::Tile(float startValue)
{
	this->height = startValue;
	this->obstacle = false;
}
Tile::~Tile()
{
}

void Tile::setObstacle(bool state)
{
	this->obstacle = state;
}
bool Tile::getObstacle() const
{
	return this->obstacle;
}

void Tile::setHeight(float height)
{
	this->height = height;
	if (this->height < 0)
		this->height = 0.0f;
	if (this->height > 255)
		this->height = 255.0f;
}
float Tile::getHeight() const
{
	return this->height;
}