#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "Loader.h"

#pragma comment (lib, "d3d11.lib")

#define HUE_DEFAULT	DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f)
#define HUE_HIT		DirectX::XMFLOAT4(2.f, 0.5f, 0.5f, 1.f)

struct InputType
{
	InputType() {}
	InputType(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz) : pos(x, y, z), tex(u, v), nor(nx, ny, nz) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tex;
	DirectX::XMFLOAT3 nor;
};

struct ObjectInstance
{
	ID3D11Buffer*		vertexBuffer;
	ID3D11Buffer*		indexBuffer;

	std::vector<DirectX::XMFLOAT4X4> world;
	std::vector<bool>	hit;

	InputType*			input;
	UINT*				indices;

	int					nVertices;
	int					nIndices;
	int					nNormals;
	int					textureIndex;

	void Delete()
	{
		if (vertexBuffer) { vertexBuffer->Release(); }
		if (indexBuffer) { indexBuffer->Release(); }
		delete[] input;
		delete[] indices;
	}
};

class ObjectManager
{
private:
	struct constBufferPerObject
	{
		DirectX::XMFLOAT4X4 WVP;
		DirectX::XMFLOAT4X4 World;
		DirectX::XMFLOAT4	Hue;
	}					cbPerObject;

	DirectX::XMFLOAT4X4 m_view;
	DirectX::XMFLOAT4X4 m_projection;

	Loader*				m_loader;

	ObjectInstance*		m_objPlayer;
	ObjectInstance*		m_objEnemies;
	ObjectInstance*		m_objObstacles;
	ObjectInstance*		m_objTiles;

	ID3D11Buffer*		cbPerObjectBuffer;
	ID3D11SamplerState* samplerState;

private:
	void InitInstances(Object obj, ObjectInstance *&object);
	void CreateBuffers(ID3D11Device* device);

	bool LoadTextures(ID3D11Device* device);
	void RenderInstances(ID3D11DeviceContext* deviceContext, ObjectInstance* arr);
	//void RenderInstances(ID3D11DeviceContext* deviceContext, ObjectInstance* arr, int size);

	void CreateSamplers(ID3D11Device* device);

public:
	ObjectManager();
	ObjectManager(const ObjectManager& obj);
	~ObjectManager();

	void Initialize(ID3D11Device* device, int nEnemies, int nObstacles, int nTiles);

	void SetPlayerHit(bool hit);
	void SetEnemyHit(int index, bool hit);

	void SetPlayerWorld(const DirectX::XMMATRIX &world);
	void SetEnemiesWorld(const DirectX::XMMATRIX* arr);
	void SetEnemiesWorld(int index, const DirectX::XMMATRIX &world);
	void SetObstaclesWorld(const DirectX::XMMATRIX* arr);
	void SetObstaclesWorld(int index, const DirectX::XMMATRIX &world);
	void SetTilesWorld(const DirectX::XMMATRIX* arr);
	void SetTileWorld(int index, const DirectX::XMMATRIX &world);

	int GetEnemyCount();
	int GetObstacleCount();
	int GetTileCount();

	void Update();
	void Render(ID3D11DeviceContext* deviceContext);

	void setViewProjection(const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &projection);

	void ReleaseCOM();
};

#endif