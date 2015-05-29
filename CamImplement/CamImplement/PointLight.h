#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include<d3d11.h>
#include<DirectXMath.h>

#define MAX_NUMBER_OF_LIGHTS 2

#define LIGHT_DEFAULT_RANGE			10.0f
#define LIGHT_DEFAULT_ATTENUATION	DirectX::XMFLOAT3(0.2f, 0.0f, 0.0f)
#define LIGHT_DEFAULT_AMBIENT		DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
#define LIGHT_DEFAULT_DIFFUSE		DirectX::XMFLOAT4(0.2f, 0.8f, 0.2f, 1.0f)

#define LIGHT_ATTACK1_DIFFUSE		DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f)
#define LIGHT_BLOCK_DIFFUSE			DirectX::XMFLOAT4(0.2f, 0.2f, 0.8f, 1.0f)

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
	float pad;
	DirectX::XMFLOAT4	ambient;
	DirectX::XMFLOAT4	diffuse;
};

class PointLight
{
private:
	Light* lightArr;
	int nLights;

public:
	PointLight();
	PointLight(const PointLight& obj);
	~PointLight();

	void Initialize(int _nLights);

	void setPosition(int index, DirectX::XMVECTOR pos);
	void setColor(int index, int action);
	void setRangeByHitPoints(int index, float hp);

	Light& getLight(int index) const;
};

#endif