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
		lightArr[i].pos = DirectX::XMFLOAT3(0.0f, 2.0f, 0.0f);
		lightArr[i].range = LIGHT_DEFAULT_RANGE;
		lightArr[i].att = LIGHT_DEFAULT_ATTENUATION;
		lightArr[i].ambient = LIGHT_DEFAULT_AMBIENT;
		lightArr[i].diffuse = LIGHT_DEFAULT_DIFFUSE;
	}
}

void PointLight::setPosition(int index, DirectX::XMVECTOR pos)
{
	//DirectX::XMStoreFloat3(&lightArr[index].pos, pos);
	lightArr[index].pos.x = DirectX::XMVectorGetX(pos);
	lightArr[index].pos.z = DirectX::XMVectorGetZ(pos);
}

Light& PointLight::getLight(int index) const
{
	return lightArr[index];
}