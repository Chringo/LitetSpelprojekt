#include "ObjectManager.h"

using namespace DirectX;
using namespace std;

ObjectManager::ObjectManager()
{
	m_loader = nullptr;
	m_objPlayer = nullptr;
	m_objEnemies = nullptr;
	m_objObstacles = nullptr;
	m_objTiles = nullptr;
	cbPerObjectBuffer = nullptr;
}

ObjectManager::ObjectManager(const ObjectManager& obj)
{

}

ObjectManager::~ObjectManager()
{
}

// *** FIX THIS ***
//Current code makes index buffer useless

void ObjectManager::InitInstances(Object obj, ObjectInstance *&object)
{
	// We only need 1 instance of each object.
	// Apply buffer -> draw multiple times using differente cBuffers.

	float x, y, z;
	float u, v;
	float nx, ny, nz;
	int iV, iT, iN;
	int nV, nT, nN, nI;

	ObjectType *temp = m_loader->getObject(obj);

	if (!temp)
		return;

	object = new ObjectInstance();

	nV = m_loader->getVertexCount(obj);
	nT = m_loader->getTextureCoordCount(obj);
	nN = m_loader->getNormalCount(obj);
	nI = m_loader->getIndexCount(obj);

	object->nIndices = nI;
	object->nVertices = nI;
	object->indices = new UINT[nI];
	object->input = new InputType[nI];

	for (int j = 0; j < nI / 3; j++)
	{
		object->indices[(j * 3)] = (j * 3);
		object->indices[(j * 3) + 1] = (j * 3) + 1;
		object->indices[(j * 3) + 2] = (j * 3) + 2;

		iV = temp->faces[j].vIndex1 - 1;
		iT = temp->faces[j].tIndex1 - 1;
		iN = temp->faces[j].nIndex1 - 1;
		x = temp->vertices[iV].x;
		y = temp->vertices[iV].y;
		z = temp->vertices[iV].z;
		u = temp->texCoords[iT].u;
		v = temp->texCoords[iT].v;
		nx = temp->normals[iN].x;
		ny = temp->normals[iN].y;
		nz = temp->normals[iN].z;
		object->input[(j * 3)] = InputType(x, y, z, u, v, nx, ny, nz);

		iV = temp->faces[j].vIndex2 - 1;
		iT = temp->faces[j].tIndex2 - 1;
		iN = temp->faces[j].nIndex2 - 1;
		x = temp->vertices[iV].x;
		y = temp->vertices[iV].y;
		z = temp->vertices[iV].z;
		u = temp->texCoords[iT].u;
		v = temp->texCoords[iT].v;
		nx = temp->normals[iN].x;
		ny = temp->normals[iN].y;
		nz = temp->normals[iN].z;
		object->input[(j * 3 + 1)] = InputType(x, y, z, u, v, nx, ny, nz);

		iV = temp->faces[j].vIndex3 - 1;
		iT = temp->faces[j].tIndex3 - 1;
		iN = temp->faces[j].nIndex3 - 1;
		x = temp->vertices[iV].x;
		y = temp->vertices[iV].y;
		z = temp->vertices[iV].z;
		u = temp->texCoords[iT].u;
		v = temp->texCoords[iT].v;
		nx = temp->normals[iN].x;
		ny = temp->normals[iN].y;
		nz = temp->normals[iN].z;
		object->input[(j * 3 + 2)] = InputType(x, y, z, u, v, nx, ny, nz);
	}

	object->textureIndex = obj;
	object->vertexBuffer = nullptr;
	object->indexBuffer = nullptr;
}

void ObjectManager::CreateBuffers(ID3D11Device* device)
{
	//Constant buffer
	D3D11_BUFFER_DESC cbObjectDesc;
	cbObjectDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbObjectDesc.ByteWidth = sizeof(constBufferPerObject);
	cbObjectDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbObjectDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbObjectDesc.MiscFlags = 0;
	cbObjectDesc.StructureByteStride = 0;

	device->CreateBuffer(&cbObjectDesc, NULL, &cbPerObjectBuffer);

	//Vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = 0;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = nullptr;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	//Index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = 0;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iData;
	iData.pSysMem = nullptr;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	//Player buffers
	if (m_objPlayer)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objPlayer->nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objPlayer->nIndices;
		vData.pSysMem = m_objPlayer->input;
		iData.pSysMem = m_objPlayer->indices;
		device->CreateBuffer(&vertexBufferDesc, &vData, &m_objPlayer->vertexBuffer);
		device->CreateBuffer(&indexBufferDesc, &iData, &m_objPlayer->indexBuffer);
	}

	//Enemy buffer
	if (m_objEnemies)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objEnemies->nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objEnemies->nIndices;
		vData.pSysMem = m_objEnemies->input;
		iData.pSysMem = m_objEnemies->indices;
		device->CreateBuffer(&vertexBufferDesc, &vData, &m_objEnemies->vertexBuffer);
		device->CreateBuffer(&indexBufferDesc, &iData, &m_objEnemies->indexBuffer);
	}

	//Obstacle buffers
	if (m_objObstacles)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objObstacles->nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objObstacles->nIndices;
		vData.pSysMem = m_objObstacles->input;
		iData.pSysMem = m_objObstacles->indices;
		device->CreateBuffer(&vertexBufferDesc, &vData, &m_objObstacles->vertexBuffer);
		device->CreateBuffer(&indexBufferDesc, &iData, &m_objObstacles->indexBuffer);
	}

	//Tile buffers
	if (m_objTiles)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objTiles->nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objTiles->nIndices;
		vData.pSysMem = m_objTiles->input;
		iData.pSysMem = m_objTiles->indices;
		device->CreateBuffer(&vertexBufferDesc, &vData, &m_objTiles->vertexBuffer);
		device->CreateBuffer(&indexBufferDesc, &iData, &m_objTiles->indexBuffer);
	}
}

void ObjectManager::CreateSamplers(ID3D11Device* device)
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sampDesc, &samplerState);
}

void ObjectManager::RenderInstances(ID3D11DeviceContext* deviceContext, ObjectInstance* arr)
{
	if (!arr)
		return;

	UINT stride = sizeof(InputType);
	UINT offset = 0;
	ID3D11ShaderResourceView* srv = nullptr;

	// Set object buffer & textures.
	deviceContext->IASetVertexBuffers(0, 1, &arr->vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(arr->indexBuffer, DXGI_FORMAT_R32_UINT, offset);

	srv = m_loader->getTexture(arr->textureIndex);
	deviceContext->PSSetShaderResources(0, 1, &srv);

	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&m_view);
	DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&m_projection);

	// Draw buffers for each world matrix.
	for (UINT i = 0; i < arr->world.size(); i++)
	{
		// Update buffers & textures.
		XMMATRIX world = XMLoadFloat4x4(&arr->world[i]);
		XMMATRIX wvp = world * view * projection;
		XMStoreFloat4x4(&cbPerObject.World, XMMatrixTranspose(world));
		XMStoreFloat4x4(&cbPerObject.WVP, XMMatrixTranspose(wvp));

		D3D11_MAPPED_SUBRESOURCE cb;
		ZeroMemory(&cb, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deviceContext->Map(cbPerObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
		memcpy(cb.pData, &cbPerObject, sizeof(constBufferPerObject));
		deviceContext->Unmap(cbPerObjectBuffer, 0);

		// Pass data to shaders.
		deviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

		// Draw mesh.
		deviceContext->DrawIndexed(arr->nIndices, 0, 0);
	}
}

void ObjectManager::Initialize(ID3D11Device* device, int nEnemies, int nObstacles, int nTiles)
{
	m_loader = new Loader();
	Object obj[] = { Player, Enemy, Obstacle, Tile };
	m_loader->Initialize(device, obj, (sizeof(obj) / sizeof(Object)));

	// Create meshes & buffers.
	InitInstances(Player, m_objPlayer);
	InitInstances(Enemy, m_objEnemies);
	InitInstances(Obstacle, m_objObstacles);
	InitInstances(Tile, m_objTiles);

	CreateBuffers(device);
	CreateSamplers(device);

	// Create instances.
	XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, XMMatrixIdentity());

	m_objPlayer->world.push_back(mat);

	for (INT i = 0; i < nEnemies; i++)
		m_objEnemies->world.push_back(mat);
	
	for (INT i = 0; i < nObstacles; i++)
		m_objObstacles->world.push_back(mat);

	for (INT i = 0; i < nTiles; i++)
		m_objTiles->world.push_back(mat);

	XMStoreFloat4x4(&cbPerObject.World, XMMatrixIdentity());
	XMStoreFloat4x4(&cbPerObject.WVP, XMMatrixIdentity());
}

void ObjectManager::SetPlayerWorld(const XMMATRIX &world)
{
	XMStoreFloat4x4(&m_objPlayer->world[0], world);
}

void ObjectManager::SetEnemiesWorld(const XMMATRIX* arr)
{
	for (UINT i = 0; i < m_objEnemies->world.size(); i++)
	{
		SetEnemiesWorld(i, arr[i]);
	}
}

void ObjectManager::SetEnemiesWorld(int index, const XMMATRIX &world)
{
	//Passing world directly into StoreFloat causes random access violation
	XMMATRIX w = world;
	XMStoreFloat4x4(&m_objEnemies->world[index], w);
}

void ObjectManager::SetObstaclesWorld(const XMMATRIX* arr)
{
	for (UINT i = 0; i < m_objObstacles->world.size(); i++)
	{
		SetObstaclesWorld(i, arr[i]);
	}
}

void ObjectManager::SetObstaclesWorld(int index, const XMMATRIX &world)
{
	//Passing world directly into StoreFloat causes random access violation
	XMMATRIX w = world;
	XMStoreFloat4x4(&m_objTiles->world[index], w);
}

void ObjectManager::SetTilesWorld(const XMMATRIX* arr)
{
	for (UINT i = 0; i < m_objTiles->world.size(); i++)
	{
		SetTileWorld(i, arr[i]);
	}
}

void ObjectManager::SetTileWorld(int index, const XMMATRIX &world)
{
	//Passing world directly into StoreFloat causes random access violation
	XMMATRIX w = world;
	XMStoreFloat4x4(&m_objTiles->world[index], w);
}

void ObjectManager::Update()
{
	
}

void ObjectManager::Render(ID3D11DeviceContext* deviceContext)
{
	deviceContext->PSSetSamplers(0, 1, &samplerState);

	RenderInstances(deviceContext, m_objPlayer);
	RenderInstances(deviceContext, m_objEnemies);
	RenderInstances(deviceContext, m_objObstacles);
	RenderInstances(deviceContext, m_objTiles);
}

void ObjectManager::setViewProjection(const XMMATRIX &view, const XMMATRIX &projection)
{
	XMStoreFloat4x4(&m_view, view);
	XMStoreFloat4x4(&m_projection, projection);
}

void ObjectManager::ReleaseCOM()
{
	m_loader->ReleaseCOM();

	if (m_objPlayer)
	{
		m_objPlayer->Delete();
		delete m_objPlayer;
	}
	if (m_objEnemies)
	{
		m_objEnemies->Delete();
		delete m_objEnemies;
	}
	if (m_objTiles)
	{
		m_objTiles->Delete();
		delete m_objTiles;
	}
	if (m_objObstacles)
	{
		m_objObstacles->Delete();
		delete m_objObstacles;
	}

	cbPerObjectBuffer->Release();
	samplerState->Release();
}