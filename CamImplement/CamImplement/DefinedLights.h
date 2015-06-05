#pragma once

enum DiffuseType
{
	Player0,
	Enemy1,
	Enemy2,
	Enemy3
};

#define MAX_NUMBER_OF_LIGHTS 10

#define LIGHT_DEFAULT_RANGE				10.0f
#define LIGHT_DEFAULT_ATTENUATION		DirectX::XMFLOAT3(0.2f, 0.0f, 0.0f)
#define LIGHT_DEFAULT_AMBIENT			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
#define LIGHT_PLAYERDEFAULT0_DIFFUSE	DirectX::XMFLOAT4(0.2f, 0.8f, 0.2f, 1.0f)
#define LIGHT_ENEMYDEFAULT1_DIFFUSE		DirectX::XMFLOAT4(0.8f, 0.2f, 0.2f, 1.0f)
#define LIGHT_ENEMYDEFAULT2_DIFFUSE		DirectX::XMFLOAT4(0.8f, 0.8f, 0.2f, 1.0f)
#define LIGHT_ENEMYDEFAULT3_DIFFUSE		DirectX::XMFLOAT4(0.8f, 0.2f, 0.8f, 1.0f)

#define LIGHT_ATTACK1_DIFFUSE			DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f)
#define LIGHT_BLOCK_DIFFUSE				DirectX::XMFLOAT4(0.2f, 0.2f, 0.8f, 1.0f)