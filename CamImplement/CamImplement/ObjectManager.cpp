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

// *** FIX THIS ***
//Normals and texture coordinates are not loaded in the proper place
void ObjectManager::InitInstances(Object obj, ObjectInstance** arr, int size)
{
	if (size <= 0) { return; }

	*arr = new ObjectInstance[size];

	float x, y, z;
	float u, v;
	float nx, ny, nz;

	int pos_i, tex_i, nor_i;

	ObjectType temp = m_loader->getObject(obj);
	for (int i = 0; i < size; i++)
	{
		(*arr)[i].nVertices = m_loader->getVertexCount(obj);
		(*arr)[i].nIndices = m_loader->getIndexCount(obj);

		(*arr)[i].input = new InputType[(*arr)[i].nVertices];
		(*arr)[i].indices = new UINT[(*arr)[i].nIndices];

		int k = 0;
		for (int j = 0; j < (*arr)[i].nIndices / 3; j++)
		{
			(*arr)[i].indices[k] = temp.faces[j].vIndex1 - 1;
			(*arr)[i].indices[k + 1] = temp.faces[j].vIndex2 - 1;
			(*arr)[i].indices[k + 2] = temp.faces[j].vIndex3 - 1;
			k += 3;
		}

		for (int j = 0; j < (*arr)[i].nVertices; j++)
		{	
			pos_i = j;
			tex_i = -1;
			nor_i = -1;

			//Fix this
			for (k = 0; (tex_i == -1 || nor_i == -1) && k < (*arr)[i].nIndices / 3; k++)
			{
				if (pos_i == temp.faces[k].vIndex1 - 1)
				{
					tex_i = temp.faces[k].tIndex1 - 1;
					nor_i = temp.faces[k].nIndex1 - 1;
				}
				else if (pos_i == temp.faces[k].vIndex2 - 1)
				{
					tex_i = temp.faces[k].tIndex2 - 1;
					nor_i = temp.faces[k].nIndex1 - 1;
				}
				else if (pos_i == temp.faces[k].vIndex3 - 1)
				{
					tex_i = temp.faces[k].tIndex3 - 1;
					nor_i = temp.faces[k].nIndex1 - 1;
				}
			}

			x = temp.vertices[pos_i].x;
			y = temp.vertices[pos_i].y;
			z = temp.vertices[pos_i].z;

			u = temp.texCoords[tex_i].u;
			v = temp.texCoords[tex_i].v;
			
			nx = temp.normals[nor_i].x;
			ny = temp.normals[nor_i].y;
			nz = temp.normals[nor_i].z;

			(*arr)[i].input[j] = InputType(x, y, z, u, v, nx, ny, nz);
		}

		(*arr)[i].textureIndex = obj;
		DirectX::XMStoreFloat4x4(&(*arr)[i].world, DirectX::XMMatrixIdentity());
		(*arr)[i].vertexBuffer = nullptr;
		(*arr)[i].indexBuffer = nullptr;
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
	vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objPlayer->nVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	int size = sizeof(*m_objPlayer->input);

	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = m_objPlayer->input;
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
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objEnemies[i].nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objEnemies[i].nIndices;
		vData.pSysMem = m_objEnemies[i].input;
		iData.pSysMem = m_objEnemies[i].indices;
		device->CreateBuffer(&vertexBufferDesc, &vData, &m_objEnemies[i].vertexBuffer);
		device->CreateBuffer(&indexBufferDesc, &iData, &m_objEnemies[i].indexBuffer);
	}

	//Obstacle buffers
	for (int i = 0; i < m_nObstacles; i++)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objObstacles[i].nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objObstacles[i].nIndices;
		vData.pSysMem = m_objObstacles[i].input;
		iData.pSysMem = m_objObstacles[i].indices;
		device->CreateBuffer(&vertexBufferDesc, &vData, &m_objObstacles[i].vertexBuffer);
		device->CreateBuffer(&indexBufferDesc, &iData, &m_objObstacles[i].indexBuffer);
	}

	//Tile buffers
	for (int i = 0; i < m_nTiles; i++)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objTiles[i].nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objTiles[i].nIndices;
		vData.pSysMem = m_objTiles[i].input;
		iData.pSysMem = m_objTiles[i].indices;
		device->CreateBuffer(&vertexBufferDesc, &vData, &m_objTiles[i].vertexBuffer);
		device->CreateBuffer(&indexBufferDesc, &iData, &m_objTiles[i].indexBuffer);
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

void ObjectManager::RenderInstances(ID3D11DeviceContext* deviceContext, ObjectInstance* arr, int size)
{
	UINT stride = sizeof(InputType);
	UINT offset = 0;
	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&m_view);
	DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&m_projection);
	ID3D11ShaderResourceView* srv;

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

		//D3D11_MAPPED_SUBRESOURCE vb;
		//ZeroMemory(&vb, sizeof(D3D11_MAPPED_SUBRESOURCE));
		//deviceContext->Map(arr[i].vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vb);
		//memcpy(vb.pData, arr[i].input, 32 * arr[i].nVertices);
		//deviceContext->Unmap(arr[i].vertexBuffer, 0);

		deviceContext->IASetIndexBuffer(arr[i].indexBuffer, DXGI_FORMAT_R32_UINT, offset);
		deviceContext->IASetVertexBuffers(0, 1, &arr[i].vertexBuffer, &stride, &offset);
		deviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

		srv = m_loader->getTexture(arr[i].textureIndex);
		deviceContext->PSSetShaderResources(0, 1, &srv);

		deviceContext->DrawIndexed(arr[i].nIndices, 0, 0);
	}
}

void ObjectManager::Initialize(ID3D11Device* device, int nEnemies, int nObstacles, int nTiles)
{
	m_loader = new Loader();
	Object obj[] = { Player, Enemy, Tile };
	m_loader->Initialize(device, obj, (sizeof(obj) / sizeof(Object)));

	m_nEnemies = nEnemies;
	m_nObstacles = nObstacles;
	m_nTiles = nTiles;

	InitInstances(Player, &m_objPlayer, 1);
	InitInstances(Enemy, &m_objEnemies, m_nEnemies);
	InitInstances(Obstacle, &m_objObstacles, m_nObstacles);
	InitInstances(Tile, &m_objTiles, m_nTiles);

	CreateBuffers(device);
	CreateSamplers(device);

	DirectX::XMStoreFloat4x4(&cbPerObject.World, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&cbPerObject.WVP, DirectX::XMMatrixIdentity());
}

void ObjectManager::SetPlayerWorld(const DirectX::XMMATRIX &world)
{
	DirectX::XMStoreFloat4x4(&m_objPlayer->world, world);
}

void ObjectManager::SetEnemiesWorld(const DirectX::XMMATRIX* arr)
{
	for (int i = 0; i < m_nEnemies; i++)
	{
		SetEnemiesWorld(i, arr[i]);
	}
}

void ObjectManager::SetEnemiesWorld(int index, const DirectX::XMMATRIX &world)
{
	//Passing world directly into StoreFloat causes random access violation
	DirectX::XMMATRIX w = world;
	DirectX::XMStoreFloat4x4(&m_objEnemies[index].world, w);
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
	//Passing world directly into StoreFloat causes random access violation
	DirectX::XMMATRIX w = world;
	DirectX::XMStoreFloat4x4(&m_objTiles[index].world, w);
}

void ObjectManager::Update()
{
	
}

void ObjectManager::Render(ID3D11DeviceContext* deviceContext)
{
	deviceContext->PSSetSamplers(0, 1, &samplerState);

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
	m_loader->ReleaseCOM();

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
	samplerState->Release();
}