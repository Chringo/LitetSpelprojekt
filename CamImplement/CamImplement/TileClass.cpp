#include "TileClass.h"

TileClass::TileClass()
{
	this->height = 0;
	this->base.obstacle = false;

	this->base.worldpos.x = 0.0f;
	this->base.worldpos.y = 0.0f;
	this->base.worldpos.z = 0.0f;

	this->type = Grass;
}
TileClass::TileClass(float startValue)
{
	this->height = startValue;
	this->base.obstacle = false;

	this->base.worldpos.x = 0.0f;
	this->base.worldpos.y = 0.0f;
	this->base.worldpos.z = 0.0f;

	this->type = Grass;
}
TileClass::TileClass(float startValue, DirectX::XMFLOAT3 _worldpos)
{
	this->height = startValue;
	this->base.obstacle = false;

	this->base.worldpos = _worldpos;

	this->type = Grass;
}
TileClass::~TileClass()
{
}

BaseTile TileClass::getBase() const
{
	return this->base;
}

void TileClass::setObstacle(bool state)
{
	this->base.obstacle = state;
}
bool TileClass::getObstacle() const
{
	return this->base.obstacle;
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
	this->base.worldpos = _worldpos;
}
DirectX::XMFLOAT3  TileClass::getWorldPos() const
{
	return this->base.worldpos;
}

void TileClass::setType(int type)
{
	switch (type)
	{
	case 0:
		this->type = Grass;
		break;
	case 1:
		this->type = Water;
		break;
	case 2:
		this->type = Stone;
		break;
	default:
		this->type = Grass;
		break;
	}
}
int TileClass::getType() const
{
	switch (type)
	{
	case Grass:
		return 0;
	case Water:
		return 1;
	case Stone:
		return 2;
	}
}