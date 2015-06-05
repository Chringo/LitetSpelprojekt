#include "PointLight.h"

PointLight::PointLight()
{

}

PointLight::PointLight(const PointLight& obj)
{

}

PointLight::~PointLight()
{
	delete[] lightArr;
}

void PointLight::Initialize(int _nLights)
{
	_nLights <= MAX_NUMBER_OF_LIGHTS ? nLights = _nLights : nLights = MAX_NUMBER_OF_LIGHTS;
	lightArr = new Light[MAX_NUMBER_OF_LIGHTS];
	lightDiffuseIndex = new DiffuseType[MAX_NUMBER_OF_LIGHTS];

	for (int i = 0; i < MAX_NUMBER_OF_LIGHTS; i++)
	{
		lightArr[i].pos = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		lightArr[i].range = LIGHT_DEFAULT_RANGE;
		lightArr[i].att = LIGHT_DEFAULT_ATTENUATION;
		lightArr[i].ambient = LIGHT_DEFAULT_AMBIENT;
		i ? lightDiffuseIndex[i] = Enemy1 : lightDiffuseIndex[i] = Player0;
		lightArr[i].diffuse = getDefaultLight(i);
		lightArr[i].framecount = 0;
	}
}

void PointLight::setPosition(int index, DirectX::XMVECTOR pos)
{
	lightArr[index].pos.x = DirectX::XMVectorGetX(pos);
	lightArr[index].pos.z = DirectX::XMVectorGetZ(pos);
}

void PointLight::setColor(int index, DirectX::XMFLOAT4 color)
{
	lightArr[index].diffuse = color;
}

void PointLight::setColor(int index, int action)
{
	switch (action)
	{
	case Attack1:

	case Attack2:
		setColor(index, LIGHT_ATTACK1_DIFFUSE);
		break;
	case Block:

	case Dodge:
		setColor(index, LIGHT_BLOCK_DIFFUSE);
		break;
	case Idle:
		
	default:
		setColor(index, getDefaultLight(index));
		if (!index && lightArr[index].framecount)
		{
			lightArr[index].framecount++;
			lightArr[index].framecount %= 60;
			int m = lightArr[index].framecount;
			if (m < 10)
			{
				setColor(index, LIGHT_ENEMYDEFAULT1_DIFFUSE);
			}
		}
		break;
	}
}

void PointLight::setRangeByHitPoints(int index, float hp)
{
	if (hp > 0)
		lightArr[index].range = LIGHT_DEFAULT_RANGE * (hp / 100.f);
	else
		lightArr[index].range = 0.f;

	if (!index && !lightArr[index].framecount && hp < 60.f)
		lightArr[index].framecount++;
}

void PointLight::setDefaultLight(int index, DiffuseType color)
{
	lightDiffuseIndex[index] = color;
}

DirectX::XMFLOAT4& PointLight::getDefaultLight(int index) const
{
	DirectX::XMFLOAT4 color = LIGHT_ENEMYDEFAULT1_DIFFUSE;

	switch (lightDiffuseIndex[index])
	{
	case Player0:
		color = LIGHT_PLAYERDEFAULT0_DIFFUSE;
		break;
	case Enemy2:
		color = LIGHT_ENEMYDEFAULT2_DIFFUSE;
		break;
	case Enemy3:
		color = LIGHT_ENEMYDEFAULT3_DIFFUSE;
		break;
	case Enemy1:

	default:
		break;
	}

	return color;
}

Light& PointLight::getLight(int index) const
{
	return lightArr[index];
}