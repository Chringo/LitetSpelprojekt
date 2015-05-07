#ifndef TILE_H
#define TILE_H

#include <DirectXMath.h>

class TileClass
{
private:
	bool obstacle;

	float height;

	DirectX::XMFLOAT3 worldpos;

	enum Type
	{
		Dummy
		/*
		Grass1,
		Grass2,
		Mud,
		Stone,
		Moss,
		Leaves
		*/
	};

public:
	TileClass();
	TileClass(float startValue);
	TileClass(float startValue, DirectX::XMFLOAT3 worldpos);
	~TileClass();

	void setObstacle(bool obstacle);
	bool getObstacle() const;

	void setHeight(float height);
	float getHeight() const;

	void setWorldPos(DirectX::XMFLOAT3 worldpos);
	DirectX::XMFLOAT3 getWorldPos() const;
};
#endif