#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include<d3d11.h>
#include<DirectXMath.h>

#define DIRLIGHT_DEFAULT_DIRECTION	DirectX::XMFLOAT3(0.0f, 1.0f, -1.0f)
#define DIRLIGHT_DEFAULT_AMBIENT	DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)
#define DIRLIGHT_DEFAULT_DIFFUSE	DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f)

struct DirLight
{
	DirectX::XMFLOAT3 dir;
	float pad;
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
};

class DirectionalLight
{
private:
	DirLight light;

public:
	DirectionalLight();
	DirectionalLight(const DirectionalLight& obj);
	~DirectionalLight();

	void Initialize(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse);

	DirLight getLight();
};

#endif