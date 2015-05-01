#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <d3d11.h>
#include <DirectXMath.h>
#include "Loader.h"

#pragma comment (lib, "d3d11.lib")

struct ObjectInstance
{
	Object obj;
	DirectX::XMFLOAT3 position;
	DirectX::XMMATRIX world;

	VertexType*	vertices;
	UINT*		indices;
	NormalType*	normals;

	int nVertices;
	int nIndices;
	int nNormals;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
};

class ObjectManager
{
private:
	struct constBufferPerObject
	{
		DirectX::XMFLOAT4X4 WVP;
		DirectX::XMFLOAT4X4 World;
	} cbPerObject;

	DirectX::XMMATRIX m_view;
	DirectX::XMMATRIX m_projection;

	Loader* loader;

	ObjectInstance* m_objInstances;
	int nObjects;

	ID3D11Buffer* cbPerObjectBuffer;

private:
	void InitVertices();
	void SetVertices(int index, const ObjectType& obj);
	void CreateBuffers(ID3D11Device* device);

public:
	ObjectManager();
	ObjectManager(const ObjectManager& obj);
	~ObjectManager();

	void Initialize(ID3D11Device* device);

	void Update(DirectX::XMFLOAT3 pos, const DirectX::XMMATRIX &world);
	void Render(ID3D11DeviceContext* deviceContext, const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &projection);

	void setViewProjection(const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &projection);
};

#endif