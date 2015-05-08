#include "ObjectManager.h"

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

void ObjectManager::InitInstances(Object obj, ObjectInstance** arr, int size)
{
	if (size <= 0) { return; }

	*arr = new ObjectInstance[size];

	ObjectType temp = m_loader->getObject(obj);
	for (int i = 0; i < size; i++)
	{
		(*arr)[i].nVertices = m_loader->getVertexCount(obj);
		(*arr)[i].nNormals = m_loader->getVertexCount(obj);
		(*arr)[i].nIndices = m_loader->getIndexCount(obj);

		(*arr)[i].vertices = new VertexType[(*arr)[i].nVertices];
		(*arr)[i].normals = new NormalType[(*arr)[i].nNormals];
		(*arr)[i].indices = new UINT[(*arr)[i].nIndices];

		for (int j = 0; j < (*arr)[i].nVertices; j++)
		{
			(*arr)[i].vertices[j] = temp.vertices[j];
		}
		for (int j = 0; j < (*arr)[i].nNormals; j++)
		{
			(*arr)[i].normals[j] = temp.normals[j];
		}

		int k = 0;
		for (int j = 0; j < (*arr)[i].nIndices / 3; j++)
		{
			(*arr)[i].indices[k] = temp.faces[j].vIndex1 - 1;
			(*arr)[i].indices[k + 1] = temp.faces[j].vIndex2 - 1;
			(*arr)[i].indices[k + 2] = temp.faces[j].vIndex3 - 1;
			k += 3;
		}

		DirectX::XMStoreFloat4x4(&(*arr)[i].world, DirectX::XMMatrixIdentity());
		(*arr)[i].vertexBuffer = nullptr;
		(*arr)[i].indexBuffer = nullptr;
		(*arr)[i].texture = nullptr;
	}
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
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_objPlayer->nVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = m_objPlayer->vertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	//Index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * m_objPlayer->nIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iData;
	iData.pSysMem = m_objPlayer->indices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	//Player buffers
	device->CreateBuffer(&vertexBufferDesc, &vData, &m_objPlayer->vertexBuffer);
	device->CreateBuffer(&indexBufferDesc, &iData, &m_objPlayer->indexBuffer);

	//Enemy buffers
	for (int i = 0; i < m_nEnemies; i++)
	{
		vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_objEnemies[i].nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objEnemies[i].nIndices;
		vData.pSysMem = m_objEnemies[i].vertices;
		iData.pSysMem = m_objEnemies[i].indices;
		device->CreateBuffer(&vertexBufferDesc, &vData, &m_objEnemies[i].vertexBuffer);
		device->CreateBuffer(&indexBufferDesc, &iData, &m_objEnemies[i].indexBuffer);
	}

	//Obstacle buffers
	for (int i = 0; i < m_nObstacles; i++)
	{
		vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_objObstacles[i].nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objObstacles[i].nIndices;
		vData.pSysMem = m_objObstacles[i].vertices;
		iData.pSysMem = m_objObstacles[i].indices;
		device->CreateBuffer(&vertexBufferDesc, &vData, &m_objObstacles[i].vertexBuffer);
		device->CreateBuffer(&indexBufferDesc, &iData, &m_objObstacles[i].indexBuffer);
	}

	//Tile buffers
	for (int i = 0; i < m_nTiles; i++)
	{
		vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_objTiles[i].nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objTiles[i].nIndices;
		vData.pSysMem = m_objTiles[i].vertices;
		iData.pSysMem = m_objTiles[i].indices;
		device->CreateBuffer(&vertexBufferDesc, &vData, &m_objTiles[i].vertexBuffer);
		device->CreateBuffer(&indexBufferDesc, &iData, &m_objTiles[i].indexBuffer);
	}
}

void ObjectManager::RenderInstances(ID3D11DeviceContext* deviceContext, ObjectInstance* arr, int size)
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&m_view);
	DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&m_projection);

	for (int i = 0; i < size; i++)
	{
		DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&arr[i].world);
		DirectX::XMMATRIX wvp = world * view * projection;
		DirectX::XMStoreFloat4x4(&cbPerObject.World, DirectX::XMMatrixTranspose(world));
		DirectX::XMStoreFloat4x4(&cbPerObject.WVP, DirectX::XMMatrixTranspose(wvp));

		D3D11_MAPPED_SUBRESOURCE cb;
		ZeroMemory(&cb, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deviceContext->Map(cbPerObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
		memcpy(cb.pData, &cbPerObject, sizeof(constBufferPerObject));
		deviceContext->Unmap(cbPerObjectBuffer, 0);

		D3D11_MAPPED_SUBRESOURCE vb;
		ZeroMemory(&vb, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deviceContext->Map(arr[i].vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vb);
		memcpy(vb.pData, arr[i].vertices, sizeof(VertexType) * arr[i].nVertices);
		deviceContext->Unmap(arr[i].vertexBuffer, 0);

		deviceContext->IASetIndexBuffer(arr[i].indexBuffer, DXGI_FORMAT_R32_UINT, offset);
		deviceContext->IASetVertexBuffers(0, 1, &arr[i].vertexBuffer, &stride, &offset);
		deviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
		deviceContext->DrawIndexed(arr[i].nIndices, 0, 0);
	}
}

void ObjectManager::Initialize(ID3D11Device* device, int nEnemies, int nObstacles, int nTiles)
{
	m_loader = new Loader();
	Object obj[] = { Player, Enemy, Tile };
	m_loader->Initialize(obj, (sizeof(obj) / sizeof(Object)));

	m_nEnemies = nEnemies;
	m_nObstacles = nObstacles;
	m_nTiles = nTiles;

	InitInstances(Player, &m_objPlayer, 1);
	InitInstances(Enemy, &m_objEnemies, m_nEnemies);
	InitInstances(Obstacle, &m_objObstacles, m_nObstacles);
	InitInstances(Tile, &m_objTiles, m_nTiles);

	CreateBuffers(device);

	DirectX::XMStoreFloat4x4(&cbPerObject.World, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&cbPerObject.WVP, DirectX::XMMatrixIdentity());
}

void ObjectManager::SetPlayerWorld(const DirectX::XMMATRIX &world)
{
	DirectX::XMStoreFloat4x4(&m_objPlayer->world, world);
}

void ObjectManager::SetEnemiesWorld(const DirectX::XMMATRIX* arr)
{
	int size = sizeof(arr) / sizeof(DirectX::XMMATRIX);
	if (size > m_nEnemies)
	{
		size = m_nEnemies;
	}

	for (int i = 0; i < size; i++)
	{
		SetEnemiesWorld(i, arr[i]);
	}
}

void ObjectManager::SetEnemiesWorld(int index, const DirectX::XMMATRIX &world)
{
	DirectX::XMStoreFloat4x4(&m_objEnemies[index].world, world);
}

void ObjectManager::SetObstaclesWorld(const DirectX::XMMATRIX* arr)
{
	int size = sizeof(arr) / sizeof(DirectX::XMMATRIX);
	if (size > m_nObstacles)
	{
		size = m_nObstacles;
	}

	for (int i = 0; i < size; i++)
	{
		SetObstaclesWorld(i, arr[i]);
	}
}

void ObjectManager::SetObstaclesWorld(int index, const DirectX::XMMATRIX &world)
{
	DirectX::XMStoreFloat4x4(&m_objObstacles[index].world, world);
}

void ObjectManager::SetTilesWorld(const DirectX::XMMATRIX* arr)
{
	for (int i = 0; i < m_nTiles; i++)
	{
		SetTileWorld(i, arr[i]);
	}
}

void ObjectManager::SetTileWorld(int index, const DirectX::XMMATRIX &world)
{
	DirectX::XMStoreFloat4x4(&m_objTiles[index].world, world);
}

void ObjectManager::Update()
{
	
}

void ObjectManager::Render(ID3D11DeviceContext* deviceContext)
{
	RenderInstances(deviceContext, m_objPlayer, 1);
	RenderInstances(deviceContext, m_objEnemies, m_nEnemies);
	RenderInstances(deviceContext, m_objObstacles, m_nObstacles);
	RenderInstances(deviceContext, m_objTiles, m_nTiles);
}

void ObjectManager::setViewProjection(const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &projection)
{
	DirectX::XMStoreFloat4x4(&m_view, view);
	DirectX::XMStoreFloat4x4(&m_projection, projection);
}

void ObjectManager::ReleaseCOM()
{
	m_objPlayer->Delete();
	delete m_objPlayer;

	for (int i = 0; i < m_nEnemies; i++)
	{
		m_objEnemies[i].Delete();
	}
	delete[] m_objEnemies;

	for (int i = 0; i < m_nObstacles; i++)
	{
		m_objObstacles[i].Delete();
	}
	delete[] m_objObstacles;

	for (int i = 0; i < m_nTiles; i++)
	{
		m_objTiles[i].Delete();
	}
	delete[] m_objTiles;

	cbPerObjectBuffer->Release();
}