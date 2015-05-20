#include "PointLight.h"

PointLight::PointLight()
{

}

PointLight::PointLight(const PointLight& obj)
{

}

PointLight::~PointLight()
{

}

void PointLight::Initialize(int _nLights)
{
	nLights = _nLights;
	lightArr = new Light[nLights];

	for (int i = 0; i < nLights; i++)
	{
		lightArr[i].pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		lightArr[i].range = LIGHT_DEFAULT_RANGE;
		lightArr[i].att = LIGHT_DEFAULT_ATTENUATION;
		lightArr[i].ambient = LIGHT_DEFAULT_AMBIENT;
		lightArr[i].diffuse = LIGHT_DEFAULT_DIFFUSE;
	}
}

void PointLight::setPosition(int index, DirectX::XMVECTOR pos)
{
	lightArr[index].pos.x = DirectX::XMVectorGetX(pos);
	lightArr[index].pos.z = DirectX::XMVectorGetZ(pos);
}

void PointLight::setColor(int index, int action)
{
	switch (action)
	{
	case Attack1:

	case Attack2:
		lightArr[index].diffuse = LIGHT_ATTACK_DIFFUSE;
		break;
	case Idle:
		lightArr[index].diffuse = LIGHT_DEFAULT_DIFFUSE;
		break;
	default:
		break;
	}
}

void PointLight::setRangeByHitPoints(int index, float hp)
{
	if (hp > 0)
	{
		lightArr[index].range = LIGHT_DEFAULT_RANGE * (hp / 100.0f);
	}
}

Light& PointLight::getLight(int index) const
{
	return lightArr[index];
}