#include "TileClass.h"

TileClass::TileClass()
{
	this->height = 200;
	this->obstacle = false;
}
TileClass::TileClass(float startValue)
{
	this->height = startValue;
	this->obstacle = false;
}
TileClass::TileClass(float startValue, DirectX::XMFLOAT3 _worldpos)
{
	this->height = startValue;
	this->obstacle = false;

	this->worldpos = _worldpos;
}
TileClass::~TileClass()
{
}

void TileClass::setObstacle(bool state)
{
	this->obstacle = state;
}
bool TileClass::getObstacle() const
{
	return this->obstacle;
}

void TileClass::setHeight(float height)
{
	this->height = height;
	if (this->height < 0)
		this->height = 0.0f;
	if (this->height > 255)
		this->height = 255.0f;
}
float TileClass::getHeight() const
{
	return this->height;
}

void TileClass::setWorldPos(DirectX::XMFLOAT3 _worldpos)
{
	this->worldpos = _worldpos;
}
DirectX::XMFLOAT3  TileClass::getWorldPos() const
{
	return this->worldpos;
}