#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "Loader.h"

#define KEYDOWN(vkey)	(GetAsyncKeyState(vkey) & 0x1)

#pragma comment (lib, "d3d11.lib")

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
	}					cbPerObject;

	DirectX::XMFLOAT4X4 m_view;
	DirectX::XMFLOAT4X4 m_projection;

	Loader*				m_loader;

	ObjectInstance*		m_objPlayer;
	ObjectInstance*		m_objEnemies;
	ObjectInstance*		m_objObstacles;
	ObjectInstance*		m_objTiles;
	ObjectInstance*		m_objMenu;
	ObjectInstance*		m_objArrow;
	int					m_objArrowStateSize;
	int					currentState;
	DirectX::XMFLOAT2*	m_objArrowPosState;

	ID3D11Buffer*		cbPerObjectBuffer;
	ID3D11SamplerState* samplerState;

private:
	void InitInstances(Object obj, ObjectInstance *&object);
	void CreateBuffers(ID3D11Device* device);

	bool LoadTextures(ID3D11Device* device);
	void RenderInstances(ID3D11DeviceContext* deviceContext, ObjectInstance* arr);

	void CreateSamplers(ID3D11Device* device);

public:
	ObjectManager();
	ObjectManager(const ObjectManager& obj);
	~ObjectManager();

	void Initialize(ID3D11Device* device, int nEnemies, int nObstacles, int nTiles);

	void SetPlayerWorld(const DirectX::XMMATRIX &world);
	void SetEnemiesWorld(const DirectX::XMMATRIX* arr);
	void SetEnemiesWorld(int index, const DirectX::XMMATRIX &world);
	void SetObstaclesWorld(const DirectX::XMMATRIX* arr);
	void SetObstaclesWorld(int index, const DirectX::XMMATRIX &world);
	void SetTilesWorld(const DirectX::XMMATRIX* arr);
	void SetTileWorld(int index, const DirectX::XMMATRIX &world);
	void SetGUIWorld (const DirectX::XMMATRIX &world);

	void Update();
	void Render(ID3D11DeviceContext* deviceContext);

	void setViewProjection(const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &projection);

	void ReleaseCOM();
};

#endif