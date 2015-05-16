#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
	light.dir = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	light.pad = 0.0f;
	light.ambient = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	light.diffuse = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

DirectionalLight::DirectionalLight(const DirectionalLight& obj)
{
	this->light = obj.light;
}

DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::Initialize(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse)
{
	light.dir = dir;
	light.ambient = ambient;
	light.diffuse = diffuse;
}

DirLight DirectionalLight::getLight()
{
	return light;
}