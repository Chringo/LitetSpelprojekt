#include "ObjectManager.h"

ObjectManager::ObjectManager()
{
	loader = nullptr;
	m_objInstances = nullptr;
}

ObjectManager::ObjectManager(const ObjectManager& obj)
{

}

ObjectManager::~ObjectManager()
{

}

void ObjectManager::Initialize(ID3D11Device* device)
{
	loader = new Loader();
	Object obj[] = { Player, Enemy };
	nObjects = sizeof(obj) / sizeof(Object);
	loader->Initialize(obj, nObjects);

	m_objInstances = new ObjectInstance[nObjects];
	for (int i = 0; i < nObjects; i++)
	{
		m_objInstances[i].obj = obj[i];
		m_objInstances[i].position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMStoreFloat4x4(&m_objInstances[i].world, DirectX::XMMatrixIdentity());
	}

	InitVertices();
	CreateBuffers(device);

	DirectX::XMStoreFloat4x4(&cbPerObject.World, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&cbPerObject.WVP, DirectX::XMMatrixIdentity());
}

void ObjectManager::Update(DirectX::XMFLOAT3 pos, const DirectX::XMMATRIX &world)
{
	m_objInstances[0].position = pos;
	DirectX::XMStoreFloat4x4(&m_objInstances[0].world, world);
	SetVertices(0, loader->getObject(Player));
}

void ObjectManager::Render(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&m_view);
	DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&m_projection);
	for (int i = 0; i < nObjects; i++)
	{
		DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&m_objInstances[i].world);
		DirectX::XMMATRIX wvp = world * view * projection;
		DirectX::XMStoreFloat4x4(&cbPerObject.World, DirectX::XMMatrixTranspose(world)); 
		DirectX::XMStoreFloat4x4(&cbPerObject.WVP, DirectX::XMMatrixTranspose(wvp));

		if (i == 1 && m_objInstances[0].position.x > 3)
		{
			int x = 0;
		}

		D3D11_MAPPED_SUBRESOURCE cb;
		ZeroMemory(&cb, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deviceContext->Map(cbPerObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
		memcpy(cb.pData, &cbPerObject, sizeof(constBufferPerObject));
		deviceContext->Unmap(cbPerObjectBuffer, 0);

		D3D11_MAPPED_SUBRESOURCE vb;
		ZeroMemory(&vb, sizeof(D3D11_MAPPED_SUBRESOURCE));
		deviceContext->Map(m_objInstances[i].vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vb);
		memcpy(vb.pData, m_objInstances[i].vertices, sizeof(VertexType) * m_objInstances[i].nVertices);
		deviceContext->Unmap(m_objInstances[i].vertexBuffer, 0);

		deviceContext->IASetIndexBuffer(m_objInstances[i].indexBuffer, DXGI_FORMAT_R32_UINT, offset);
		deviceContext->IASetVertexBuffers(0, 1, &m_objInstances[i].vertexBuffer, &stride, &offset);
		deviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
		deviceContext->DrawIndexed(m_objInstances[i].nIndices, 0, 0);
	}
}

void ObjectManager::setViewProjection(const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &projection)
{
	DirectX::XMStoreFloat4x4(&m_view, view);
	DirectX::XMStoreFloat4x4(&m_projection, projection);
}

void ObjectManager::InitVertices()
{
	ObjectType temp;
	for (int i = 0; i < nObjects; i++)
	{
		temp = loader->getObject(m_objInstances[i].obj);

		m_objInstances[i].nVertices = loader->getVertexCount(m_objInstances[i].obj);
		m_objInstances[i].nNormals = loader->getVertexCount(m_objInstances[i].obj);
		m_objInstances[i].vertices = new VertexType[m_objInstances[i].nVertices];
		m_objInstances[i].normals = new NormalType[m_objInstances[i].nNormals];
		SetVertices(i, temp);

		m_objInstances[i].nIndices = loader->getIndexCount(m_objInstances[i].obj);
		m_objInstances[i].indices = new UINT[m_objInstances[i].nIndices];

		int k = 0;
		for (int j = 0; j < m_objInstances[i].nIndices / 3; j++)
		{
			m_objInstances[i].indices[k] = temp.faces[j].vIndex1 - 1;
			m_objInstances[i].indices[k + 1] = temp.faces[j].vIndex2 - 1;
			m_objInstances[i].indices[k + 2] = temp.faces[j].vIndex3 - 1;
			k += 3;
		}
	}
}

void ObjectManager::SetVertices(int index, const ObjectType& obj)
{
	for (int i = 0; i < m_objInstances[index].nVertices; i++)
	{
		m_objInstances[index].vertices[i] = obj.vertices[i];
		m_objInstances[index].vertices[i].x += m_objInstances[index].position.x;
		m_objInstances[index].vertices[i].y += m_objInstances[index].position.y;
		m_objInstances[index].vertices[i].z += m_objInstances[index].position.z;
	}
	for (int i = 0; i < m_objInstances[index].nNormals; i++)
	{
		m_objInstances[index].normals[i] = obj.normals[i];
	}
}

void ObjectManager::CreateBuffers(ID3D11Device* device)
{
	for (int i = 0; i < nObjects; i++)
	{
		//Vertex buffer
		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(VertexType)*m_objInstances[i].nVertices;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vData;
		vData.pSysMem = m_objInstances[i].vertices;
		vData.SysMemPitch = 0;
		vData.SysMemSlicePitch = 0;

		device->CreateBuffer(&vertexBufferDesc, &vData, &m_objInstances[i].vertexBuffer);

		//Index buffer
		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(UINT)*m_objInstances[i].nIndices;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA iData;
		iData.pSysMem = m_objInstances[i].indices;
		iData.SysMemPitch = 0;
		iData.SysMemSlicePitch = 0;

		device->CreateBuffer(&indexBufferDesc, &iData, &m_objInstances[i].indexBuffer);
	}

	D3D11_BUFFER_DESC cbObjectDesc;
	cbObjectDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbObjectDesc.ByteWidth = sizeof(constBufferPerObject);
	cbObjectDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbObjectDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbObjectDesc.MiscFlags = 0;
	cbObjectDesc.StructureByteStride = 0;

	device->CreateBuffer(&cbObjectDesc, NULL, &cbPerObjectBuffer);
}