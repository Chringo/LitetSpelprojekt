#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include<d3d11.h>
#include<DirectXMath.h>
#include "DefinedLights.h"

enum Action
{
	Attack1,
	Attack2,
	Block,
	Dodge,
	MoveUp,
	MoveDown,
	MoveRight,
	MoveLeft,
	Idle
};

struct Light
{
	DirectX::XMFLOAT3	pos;
	float				range;
	DirectX::XMFLOAT3	att;
	int					framecount;
	DirectX::XMFLOAT4	ambient;
	DirectX::XMFLOAT4	diffuse;
};

class PointLight
{
private:
	Light* lightArr;
	DiffuseType* lightDiffuseIndex;
	int nLights;

public:
	PointLight();
	PointLight(const PointLight& obj);
	~PointLight();

	void Initialize(int _nLights);

	void setPosition(int index, DirectX::XMVECTOR pos);
	void setColor(int index, DirectX::XMFLOAT4 color);
	void setColor(int index, int action);
	void setRangeByHitPoints(int index, float hp);
	void setDefaultLight(int index, DiffuseType color);

	DirectX::XMFLOAT4& getDefaultLight(int index) const;
	Light& getLight(int index) const;
};

#endif