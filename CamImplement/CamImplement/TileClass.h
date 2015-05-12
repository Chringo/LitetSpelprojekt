#ifndef TILE_H
#define TILE_H

#include <DirectXMath.h>

struct BaseTile
{
	bool obstacle;
	DirectX::XMFLOAT3 worldpos;
};
class TileClass
{
private:
	BaseTile base;
	float height;

	enum Type
	{
		Dummy,
		Water
		/*
		Grass1,
		Grass2,
		Mud,
		Stone,
		Moss,
		Leaves
		*/
	};
	Type type;

public:
	TileClass();
	TileClass(float startValue);
	TileClass(float startValue, DirectX::XMFLOAT3 worldpos);
	virtual ~TileClass();

	BaseTile getBase() const;

	void setObstacle(bool obstacle);
	bool getObstacle() const;

	void setHeight(float height);
	float getHeight() const;

	void setWorldPos(DirectX::XMFLOAT3 worldpos);
	DirectX::XMFLOAT3 getWorldPos() const;

	void setType(int type);
	int getType() const;
};
#endif