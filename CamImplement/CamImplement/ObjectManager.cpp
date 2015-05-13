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

void ObjectManager::InitInstances(Object obj, ObjectInstance *&object)
{
	// We only need 1 instance of each object.
	// Apply buffer -> draw multiple times using differente cBuffers.

	float x, y, z;
	float u, v;
	float nx, ny, nz;

	ObjectType *temp = m_loader->getObject(obj);
	
	if (!temp)
		return;

	object = new ObjectInstance();

	object->nVertices = m_loader->getVertexCount(obj);
	object->nNormals = m_loader->getVertexCount(obj);
	object->nIndices = m_loader->getIndexCount(obj);

	object->input = new InputType[object->nVertices];
	//object->.normals = new NormalType[(*arr)[i].nNormals];
	object->indices = new UINT[object->nIndices];

	//-- Used for testing, to be removed -------------------------
	switch (obj)
	{
		case Player:
		{
			u = 1.0f;
			v = 1.0f;
			break;
		}
		case Enemy:
		{
			u = 1.0f;
			v = 0.0f;
			break;
		}
		case Tile:
		{
			u = 0.0f;
			v = 1.0f;
			break;
		}
		default:
		{
			u = 0.0f;
			v = 0.0f;
			break;
		}
	}
	//------------------------------------------------------------

	for (int j = 0; j < object->nVertices; j++)
	{
		x = temp->vertices[j].x;
		y = temp->vertices[j].y;
		z = temp->vertices[j].z;
		//u = temp->texCoords[j].u;
		//v = temp->texCoords[j].v;
		nx = temp->normals[j].x;
		ny = temp->normals[j].y;
		nz = temp->normals[j].z;

		object->input[j] = InputType(x, y, z, u, v, nx, ny, nz);
	}

	int k = 0;
	for (int j = 0; j < object->nIndices / 3; j++)
	{
		object->indices[k] = temp->faces[j].vIndex1 - 1;
		object->indices[k + 1] = temp->faces[j].vIndex2 - 1;
		object->indices[k + 2] = temp->faces[j].vIndex3 - 1;
		k += 3;
	}

	object->vertexBuffer = nullptr;
	object->indexBuffer = nullptr;
	object->texture = nullptr;
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

bool ObjectManager::LoadTextures(ID3D11Device* device)
{

	return false;
}

void ObjectManager::RenderInstances(ID3D11DeviceContext* deviceContext, ObjectInstance* arr)
{
	if (!arr)
		return;

	UINT stride = sizeof(InputType);
	UINT offset = 0;
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	XMMATRIX projection = XMLoadFloat4x4(&m_projection);

	// Set buffers.
	deviceContext->IASetIndexBuffer(arr->indexBuffer, DXGI_FORMAT_R32_UINT, offset);
	deviceContext->IASetVertexBuffers(0, 1, &arr->vertexBuffer, &stride, &offset);

	// Draw buffers for each world matrix.
	for (UINT i = 0; i < arr->world.size(); i++)
	{
		// Update constant buffer.
		XMMATRIX world = XMLoadFloat4x4(&arr->world[i]);
		XMMATRIX wvp = world * view * projection;
		XMStoreFloat4x4(&cbPerObject.World, XMMatrixTranspose(world));
		XMStoreFloat4x4(&cbPerObject.WVP, XMMatrixTranspose(wvp));

		D3D11_MAPPED_SUBRESOURCE cb;
		ZeroMemory(&cb, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deviceContext->Map(cbPerObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
		memcpy(cb.pData, &cbPerObject, sizeof(constBufferPerObject));
		deviceContext->Unmap(cbPerObjectBuffer, 0);

		deviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

		// Draw buffers.
		deviceContext->DrawIndexed(arr->nIndices, 0, 0);
	}
}

void ObjectManager::Initialize(ID3D11Device* device, int nEnemies, int nObstacles, int nTiles)
{
	m_loader = new Loader();
	Object obj[] = { Player, Enemy, Tile };
	m_loader->Initialize(obj, (sizeof(obj) / sizeof(Object)));

	// Create meshes & buffers.
	InitInstances(Player, m_objPlayer);
	InitInstances(Enemy, m_objEnemies);
	InitInstances(Obstacle, m_objObstacles);
	InitInstances(Tile, m_objTiles);

	CreateBuffers(device);

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
	UINT size = sizeof(arr) / sizeof(XMMATRIX);
	if (size > m_objObstacles->world.size())
	{
		size = m_objObstacles->world.size();
	}

	for (UINT i = 0; i < m_objObstacles->world.size(); i++)
	{
		SetObstaclesWorld(i, arr[i]);
	}
}

void ObjectManager::SetObstaclesWorld(int index, const XMMATRIX &world)
{
	XMStoreFloat4x4(&m_objObstacles->world[index], world);
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
}