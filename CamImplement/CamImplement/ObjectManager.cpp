#include "ObjectManager.h"

using namespace DirectX;
using namespace std;

int framecount = 0;

ObjectManager::ObjectManager()
{
	m_loader = nullptr;
	m_objPlayer = nullptr;
	m_objEnemies = nullptr;
	m_objObstacles = nullptr;
	m_objTiles = nullptr;
	m_objMenu = nullptr;
	m_objArrow = nullptr;
	m_objWon = nullptr;
	m_objLost = nullptr;
	m_objArrowPosState = nullptr;
	cbPerObjectBuffer = nullptr;
}

ObjectManager::ObjectManager(const ObjectManager& obj)
{

}

ObjectManager::~ObjectManager()
{
	delete[] m_objArrowPosState;
}

// *** FIX THIS ***
//Current code makes index buffer useless

void ObjectManager::InitInstances(Object obj, ObjectInstance *&object)
{
	// We only need 1 instance of each object.
	// Apply buffer -> draw multiple times using differente cBuffers.

	float x, y, z;				//per-vertex x, y, z
	float u, v;					//per-uv u & v coords
	float nx, ny, nz;			//per-normal x, y, z 
	int iV, iT, iN;				//vertex indices, tex indices, norm indices
	int nV, nT, nN, nI, nF;		//number of verts, texcoords, norms, indices, frames


	ObjectType *temp = m_loader->getObject(obj);

	if (!temp)
		return;

	object = new ObjectInstance();


	nV = m_loader->getVertexCount(obj);
	nT = m_loader->getTextureCoordCount(obj);
	nN = m_loader->getNormalCount(obj);
	nI = m_loader->getIndexCount(obj);
	nF = m_loader->getFrameCount(obj);

	object->nIndices = nI;
	object->nVertices = nI;
	object->indices = new UINT[nI];
	object->input = new InputType[nI];

	for (int j = 0; j < nI / 3; j++)
	{
		object->indices[(j * 3)]	 = (j * 3);
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

	for (int f = 0; f < nF; f++)
	{
		for (int j = 0; j < nI / 3; j++)
		{
			iV = temp->faces[j].vIndex1 - 1;
			object->fx.push_back (temp->frames[f].vertex[iV].x);
			object->fy.push_back (temp->frames[f].vertex[iV].y);
			object->fz.push_back (temp->frames[f].vertex[iV].z);

			iV = temp->faces[j].vIndex2 - 1;
			object->fx.push_back (temp->frames[f].vertex[iV].x);
			object->fy.push_back (temp->frames[f].vertex[iV].y);
			object->fz.push_back (temp->frames[f].vertex[iV].z);

			iV = temp->faces[j].vIndex3 - 1;
			object->fx.push_back (temp->frames[f].vertex[iV].x);
			object->fy.push_back (temp->frames[f].vertex[iV].y);
			object->fz.push_back (temp->frames[f].vertex[iV].z);
		}
	}


	if (object == m_objPlayer)
	{
		for (int f = 0; f < nF; f++)
		{
			for (int v = 0; v < nV; v++)
			{	
				//object->fx.push_back (temp->frames[f].vertex[v].x);
				//object->fy.push_back (temp->frames[f].vertex[v].y);
				//object->fz.push_back (temp->frames[f].vertex[v].z);

				//fx = temp->frames[f].vertex[v].x;
				//fy = temp->frames[f].vertex[v].y;
				//fz = temp->frames[f].vertex[v].z;
			}
		}
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
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = 0;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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

	//Menu buffers
	if (m_objMenu)
	{
		vertexBufferDesc.ByteWidth = sizeof (InputType) * m_objMenu->nVertices;
		indexBufferDesc.ByteWidth = sizeof (UINT) * m_objMenu->nIndices;
		vData.pSysMem = m_objMenu->input;
		iData.pSysMem = m_objMenu->indices;
		device->CreateBuffer (&vertexBufferDesc, &vData, &m_objMenu->vertexBuffer);
		device->CreateBuffer (&indexBufferDesc, &iData, &m_objMenu->indexBuffer);
	}

	//Menu Arrow buffers
	if (m_objArrow)
	{
		vertexBufferDesc.ByteWidth = sizeof (InputType) * m_objArrow->nVertices;
		indexBufferDesc.ByteWidth = sizeof (UINT) * m_objArrow->nIndices;
		vData.pSysMem = m_objArrow->input;
		iData.pSysMem = m_objArrow->indices;
		device->CreateBuffer (&vertexBufferDesc, &vData, &m_objArrow->vertexBuffer);
		device->CreateBuffer (&indexBufferDesc, &iData, &m_objArrow->indexBuffer);
	}

	//Menu won buffers
	if (m_objWon)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objWon->nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objWon->nIndices;
		vData.pSysMem = m_objWon->input;
		iData.pSysMem = m_objWon->indices;
		device->CreateBuffer(&vertexBufferDesc, &vData, &m_objWon->vertexBuffer);
		device->CreateBuffer(&indexBufferDesc, &iData, &m_objWon->indexBuffer);
	}

	//Menu lost buffers
	if (m_objLost)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objLost->nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objLost->nIndices;
		vData.pSysMem = m_objLost->input;
		iData.pSysMem = m_objLost->indices;
		device->CreateBuffer(&vertexBufferDesc, &vData, &m_objLost->vertexBuffer);
		device->CreateBuffer(&indexBufferDesc, &iData, &m_objLost->indexBuffer);
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

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

	device->CreateSamplerState(&sampDesc, &pointSampler);
}

void ObjectManager::RenderInstances(ID3D11DeviceContext* deviceContext, ObjectInstance* obj)
{
	if (!obj)
		return;

	UINT stride = sizeof(InputType);
	UINT offset = 0;
	ID3D11ShaderResourceView* srv = nullptr;

	// Set object buffer & textures.
	deviceContext->IASetVertexBuffers(0, 1, &obj->vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(obj->indexBuffer, DXGI_FORMAT_R32_UINT, offset);

	srv = m_loader->getTexture(obj->textureIndex);
	deviceContext->PSSetShaderResources(0, 1, &srv);

	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
	XMMATRIX shadowViewProjection = XMLoadFloat4x4(&m_shadowViewProjection);

	bool guiInstance = false;

	if (obj == m_objMenu || obj == m_objWon || obj == m_objLost)
	{
		world = XMMatrixScaling(0.7f, 1.15f, 1.0f) * XMMatrixTranslation(-0.08f, 0.0f, 0.0f);
		view = XMMatrixIdentity();
		projection = XMMatrixIdentity();
		guiInstance = true;
	}
	else if (obj == m_objArrow)
	{
		world = XMMatrixScaling(0.7f, 1.15f, 1.0f);
		world *= XMMatrixTranslation(m_objArrowPosState[currentState].x, m_objArrowPosState[currentState].y, 0.0f);
		view = XMMatrixIdentity();
		projection = XMMatrixIdentity();
		guiInstance = true;
	}
	else
	{
		view = DirectX::XMLoadFloat4x4(&m_view);
		projection = DirectX::XMLoadFloat4x4(&m_projection);
	}

	// Draw buffers for each world matrix.
	for (UINT i = 0; i < obj->world.size(); i++)
	{
		// Update buffers & textures.
		
		if (!guiInstance)
		{
			world = XMLoadFloat4x4(&obj->world[i]);
		}
		XMMATRIX wvp = world * view * projection;
		XMMATRIX swvp = world * shadowViewProjection;

		XMStoreFloat4x4(&cbPerObject.World, XMMatrixTranspose(world));
		XMStoreFloat4x4(&cbPerObject.WVP, XMMatrixTranspose(wvp));
		XMStoreFloat4x4(&cbPerObject.ShadowWVP, XMMatrixTranspose(swvp));

		if (obj->hit.size() > 0 && obj->hit.at(i))
		{
			cbPerObject.Hue = HUE_HIT;
		}
		else
		{
			cbPerObject.Hue = HUE_DEFAULT;
		}

		D3D11_MAPPED_SUBRESOURCE cb;
		ZeroMemory(&cb, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deviceContext->Map(cbPerObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
		memcpy(cb.pData, &cbPerObject, sizeof(constBufferPerObject));
		deviceContext->Unmap(cbPerObjectBuffer, 0);


		if (obj == m_objPlayer)
		{

			for (int j = 0; j < obj->nVertices; j++)
			{
				obj->input[j].pos.x = obj->fx.at (j + framecount * (obj->nVertices));
				obj->input[j].pos.y = obj->fy.at (j + framecount * (obj->nVertices));
				obj->input[j].pos.z = obj->fz.at (j + framecount * (obj->nVertices));
			}
			framecount < 191 ? framecount++ : framecount = 0;


			D3D11_MAPPED_SUBRESOURCE vb;
			ZeroMemory (&vb, sizeof (D3D11_MAPPED_SUBRESOURCE));
			deviceContext->Map (obj->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vb);
			memcpy (vb.pData, obj->input, sizeof (InputType) * m_objPlayer->nVertices);
			deviceContext->Unmap (obj->vertexBuffer, 0);

			deviceContext->IASetVertexBuffers (0, 1, &obj->vertexBuffer, &stride, &offset);
			deviceContext->IASetIndexBuffer (obj->indexBuffer, DXGI_FORMAT_R32_UINT, offset);
		}

		// Pass data to shaders.
		deviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

		// Draw mesh.
		deviceContext->DrawIndexed(obj->nIndices, 0, 0);
	}
}

void ObjectManager::Initialize(ID3D11Device* device, int nEnemies, int nObstacles, int nTiles)
{
	renderMenu = false;

	m_loader = new Loader();
	Object obj[] = { Player, Enemy, Obstacle, Tile, Menu, Arrow, Won, Lost };
	m_loader->Initialize(device, obj, (sizeof(obj) / sizeof(Object)));

	// Create meshes & buffers.
	InitInstances(Player, m_objPlayer);
	InitInstances(Enemy, m_objEnemies);
	InitInstances(Obstacle, m_objObstacles);
	InitInstances(Tile, m_objTiles);
	InitInstances(Menu, m_objMenu);
	InitInstances(Arrow, m_objArrow);
	InitInstances(Won, m_objWon);
	InitInstances(Lost, m_objLost);

	m_objArrowStateSize = 2;
	m_objArrowPosState = new DirectX::XMFLOAT2[m_objArrowStateSize];
	m_objArrowPosState[0] = DirectX::XMFLOAT2(0.5f, -0.7f);
	m_objArrowPosState[1] = DirectX::XMFLOAT2(0.3f, -0.9f);
	currentState = 1;

	CreateBuffers(device);
	CreateSamplers(device);

	// Create instances.
	XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, XMMatrixIdentity ());

	m_objPlayer->world.push_back(mat);
	m_objPlayer->hit.push_back(false);

	for (INT i = 0; i < nEnemies; i++)
	{
		m_objEnemies->world.push_back(mat);
		m_objEnemies->hit.push_back(false);
	}
	
	for (INT i = 0; i < nObstacles; i++)
		m_objObstacles->world.push_back(mat);

	for (INT i = 0; i < nTiles; i++)
		m_objTiles->world.push_back (mat);

	m_objMenu->world.push_back (mat);

	m_objArrow->world.push_back (mat);

	m_objWon->world.push_back(mat);

	m_objLost->world.push_back(mat);

	XMStoreFloat4x4(&cbPerObject.World, XMMatrixIdentity());
	XMStoreFloat4x4(&cbPerObject.WVP, XMMatrixIdentity());
}

void ObjectManager::SetPlayerHit(bool hit)
{
	m_objPlayer->hit[0] = hit;
}

void ObjectManager::SetEnemyHit(int index, bool hit)
{
	m_objEnemies->hit[index] = hit;
}

/***************************************************************************/

void ObjectManager::SetPlayerWorld(const XMMATRIX &world)
{
	XMStoreFloat4x4(&m_objPlayer->world[0], world);
}

void ObjectManager::SetEnemiesWorld(const XMMATRIX* arr)
{
	for (UINT i = 0; i < m_objEnemies->world.size(); i++)
	{
		XMMATRIX w = arr[i];
		XMStoreFloat4x4(&m_objEnemies->world[i], w);
	}
}

void ObjectManager::SetObstaclesWorld(const XMMATRIX* arr)
{
	for (UINT i = 0; i < m_objObstacles->world.size(); i++)
	{
		//Passing world directly into StoreFloat causes random access violation
		XMMATRIX w = arr[i];
		XMStoreFloat4x4(&m_objObstacles->world[i], w);
	}
}

void ObjectManager::SetTilesWorld(const XMMATRIX* arr)
{
	for (UINT i = 0; i < m_objTiles->world.size(); i++)
	{
		//Passing world directly into StoreFloat causes random access violation
		XMMATRIX w = arr[i];
		XMStoreFloat4x4(&m_objTiles->world[i], w);
	}
}

/***************************************************************************/

void ObjectManager::SetRenderMenu(bool render)
{
	renderMenu = render;
}

void ObjectManager::SetRenderWon(bool render)
{
	renderWon = render;
}

void ObjectManager::SetRenderLost(bool render)
{
	renderLost = render;
}

void ObjectManager::IncreaseMenuState()
{
	currentState++;
	if (currentState > m_objArrowStateSize - 1)
	{
		currentState = 0;
	}
}

void ObjectManager::DecreaseMenuState()
{
	currentState--;
	if (currentState < 0)
	{
		currentState = m_objArrowStateSize - 1;
	}
}

int ObjectManager::GetMenuState() const
{
	return currentState;
}

int ObjectManager::GetEnemyCount()
{
	return m_objEnemies->world.size();
}

int ObjectManager::GetObstacleCount()
{
	return m_objObstacles->world.size();
}

int ObjectManager::GetTileCount()
{
	return m_objTiles->world.size();
}

bool ObjectManager::GetRenderMenu() const
{
	return renderMenu;
}

bool ObjectManager::GetRenderWon() const
{
	return renderWon;
}
bool ObjectManager::GetRenderLost() const
{
	return renderLost;
}

void ObjectManager::SetAnimationState (int index, AnimationState animState)
{
	switch (animState)
	{
	case Attack:
		//From m_objPlayer->fx.at (1) to m_objPlayer->fx.at (40)
		break;
	case WalkStart:
		
		break;
	case WalkLoop:

		break;
	case WalkEnd:

		break;
	case AnBlock:

		break;
	case AnDodge:

		break;
	}
}

void ObjectManager::Update()
{
	
}

void ObjectManager::Render(ID3D11DeviceContext* deviceContext)
{
	deviceContext->PSSetSamplers(0, 1, &samplerState);


	//IMPORTANT: If you put two planes in the same matrix pos,
	//the one that is called first (Arrow vs Menu for example)
	//gets rendered over the other one.
	RenderInstances(deviceContext, m_objPlayer);
	RenderInstances(deviceContext, m_objEnemies);
	RenderInstances(deviceContext, m_objObstacles);
	RenderInstances(deviceContext, m_objTiles);
	if (renderMenu)
	{
		RenderInstances(deviceContext, m_objArrow);
		RenderInstances(deviceContext, m_objMenu);
	}
	if (renderWon)
	{
		RenderInstances(deviceContext, m_objWon);
	}
	if (renderLost)
	{
		RenderInstances(deviceContext, m_objLost);
	}
}

void ObjectManager::RenderInstanceGeometry(ID3D11DeviceContext *deviceContext, ObjectInstance *object, const XMMATRIX &viewProjection)
{
	 // Draw geometry only.
	D3D11_MAPPED_SUBRESOURCE cb;
	ZeroMemory(&cb, sizeof(D3D11_MAPPED_SUBRESOURCE));

	// Apply buffers.
	UINT stride = sizeof(InputType);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &object->vertexBuffer, &stride, &offset);

	// Draw.
	for each (XMFLOAT4X4 w in object->world)
	{
		XMMATRIX world = XMLoadFloat4x4(&w);
		XMMATRIX wvp = world * viewProjection;
		XMStoreFloat4x4(&cbPerObject.World, XMMatrixTranspose(world));
		XMStoreFloat4x4(&cbPerObject.WVP, XMMatrixTranspose(wvp));

		deviceContext->Map(cbPerObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
		memcpy(cb.pData, &cbPerObject, sizeof(constBufferPerObject));
		deviceContext->Unmap(cbPerObjectBuffer, 0);

		deviceContext->Draw(object->nVertices, 0);
	}
}

void ObjectManager::RenderGeometry(ID3D11DeviceContext *deviceContext, const XMMATRIX &viewProjection)
{
	deviceContext->PSSetSamplers(1, 1, &pointSampler);

	// Required to convert calculate distance between light source and pixel.
	XMStoreFloat4x4(&m_shadowViewProjection, viewProjection);

	RenderInstanceGeometry(deviceContext, m_objPlayer, viewProjection);
	RenderInstanceGeometry(deviceContext, m_objEnemies, viewProjection);
	RenderInstanceGeometry(deviceContext, m_objObstacles, viewProjection);
	
	// These have nothing to cast shadows on.
	//RenderInstanceGeometry(deviceContext, m_objTiles, viewProjection);
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
	if (m_objMenu)
	{
		m_objMenu->Delete ();
		delete m_objMenu;
	}
	if (m_objArrow)
	{
		m_objArrow->Delete ();
		delete m_objArrow;
	}
	if (m_objWon)
	{
		m_objWon->Delete();
		delete m_objWon;
	}
	if (m_objLost)
	{
		m_objLost->Delete();
		delete m_objLost;
	}

	cbPerObjectBuffer->Release();
	samplerState->Release();
}