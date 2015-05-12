#include "TileClass.h"

TileClass::TileClass()
{
	this->height = 0;
	this->obstacle = false;

	this->worldpos.x = 0.0f;
	this->worldpos.y = -1.0f;
	this->worldpos.z = 0.0f;

	this->type = Dummy;
}
TileClass::TileClass(float startValue)
{
	this->height = startValue;
	this->obstacle = false;

	this->worldpos.x = 0.0f;
	this->worldpos.y = -1.0f;
	this->worldpos.z = 0.0f;

	this->type = Dummy;
}
TileClass::TileClass(float startValue, DirectX::XMFLOAT3 _worldpos)
{
	this->height = startValue;
	this->obstacle = false;

	this->worldpos = _worldpos;

	this->type = Dummy;
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

void TileClass::setType(int type)
{
	this->type = (Type)type;
}
int TileClass::getType() const
{
	return (int)(this->type);
}