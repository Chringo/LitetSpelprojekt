#include "Graphics.h"

Graphics::Graphics()
{
	rSwapChain = nullptr;
	rDevice = nullptr;
	rDeviceContext = nullptr;
	rBackbufferRTV = nullptr;
	rDepthStencilView = nullptr;
	rDepthStencilBuffer = nullptr;

	loader = nullptr;
	game = nullptr;
	camera = nullptr;

	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	cbPerFrameBuffer = nullptr;
	cbPerObjectBuffer = nullptr;

	rVertexLayout = nullptr;
	vertices = nullptr;
	indices = nullptr;

	rVS = nullptr;
	rPS = nullptr;
}

Graphics::Graphics(const Graphics &obj)
{

}

Graphics::~Graphics()
{
	delete loader;
	delete game;
	delete camera;
	delete[] vertices;
	delete[] indices;
}

HRESULT Graphics::CreateDirect3DContext(HWND &wndHandle)
{
	DXGI_SWAP_CHAIN_DESC scDesc;

	ZeroMemory(&scDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	scDesc.BufferCount = 1;
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.OutputWindow = wndHandle;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Windowed = TRUE;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		NULL,
		D3D11_SDK_VERSION,
		&scDesc,
		&rSwapChain,
		&rDevice,
		nullptr,
		&rDeviceContext);


	if (SUCCEEDED(hr))
	{
		ID3D11Texture2D* pBackbuffer = nullptr;
		rSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackbuffer);

		rDevice->CreateRenderTargetView(pBackbuffer, nullptr, &rBackbufferRTV);
		pBackbuffer->Release();

		rDeviceContext->OMSetRenderTargets(1, &rBackbufferRTV, nullptr);
	}
	else
	{
		MessageBox(0, L"Couldn't create device context", L"Error", MB_OK);
	}

	return hr;
}

void Graphics::SetViewport(int width, int height)
{
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = (float)width;
	viewport.Height = (float)height;

	rDeviceContext->RSSetViewports(1, &viewport);
}

HRESULT Graphics::CreateDepthBuffer(int width, int height)
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC dsbDesc;
	ZeroMemory(&dsbDesc, sizeof(D3D11_TEXTURE2D_DESC));
	dsbDesc.Width = width;
	dsbDesc.Height = height;
	dsbDesc.MipLevels = 1;
	dsbDesc.ArraySize = 1;
	dsbDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsbDesc.SampleDesc.Count = 1;
	dsbDesc.SampleDesc.Quality = 0;
	dsbDesc.Usage = D3D11_USAGE_DEFAULT;
	dsbDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsbDesc.CPUAccessFlags = 0;
	dsbDesc.MiscFlags = 0;

	hr = rDevice->CreateTexture2D(&dsbDesc, NULL, &rDepthStencilBuffer);
	if (FAILED(hr)) { return hr; }

	rDevice->CreateDepthStencilView(rDepthStencilBuffer, NULL, &rDepthStencilView);
	if (FAILED(hr)) { return hr; }

	rDeviceContext->OMSetRenderTargets(1, &rBackbufferRTV, rDepthStencilView);
	return hr;
}

HRESULT Graphics::CreateShaders()
{
	HRESULT hr;

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ID3DBlob* pVS = nullptr;
	ID3DBlob* pPS = nullptr;

	// Create shaders
	hr = D3DCompileFromFile(L"VS.hlsl", nullptr, nullptr, "main", "vs_4_0", 0, 0, &pVS, nullptr);
	if (FAILED(hr))
	{
		MessageBox(0, L"ID3D11VertexShader* rVS", L"Failed to compile shader", MB_OK);
		return hr;
	}

	hr = D3DCompileFromFile(L"PS.hlsl", nullptr, nullptr, "main", "ps_4_0", 0, 0, &pPS, nullptr);
	if (FAILED(hr))
	{
		MessageBox(0, L"ID3D11PixelShader* rPS", L"Failed to compile shader", MB_OK);
		return hr;
	}

	// Create vertex layout
	hr = rDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &rVertexLayout);
	if (FAILED(hr))
	{
		MessageBox(0, L"ID3D11InputLayout* rVertexLayout", L"Failed to create input layout", MB_OK);
		return hr;
	}

	// Create shaders
	hr = rDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &rVS);
	if (FAILED(hr))
	{
		MessageBox(0, L"ID3D11VertexShader* rVS", L"Failed to create shader", MB_OK);
		return hr;
	}

	hr = rDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &rPS);
	if (FAILED(hr))
	{
		MessageBox(0, L"ID3D11PixelShader* rPS", L"Failed to create shader", MB_OK);
		return hr;
	}

	return hr;
}

void Graphics::InitVertices()
{
	ObjectType temp = loader->getObject(Player);
	nVertices = loader->getVertexCount(Player);
	nNormals = loader->getVertexCount(Player);
	vertices = new VertexType[nVertices];
	normals = new NormalType[nNormals];
	SetVertices(temp);
	
	nIndices = loader->getIndexCount(Player);
	indices = new UINT[nIndices];
	
	int j = 0;
	for (int i = 0; i < nIndices / 3; i++)
	{
		indices[j] = temp.faces[i].vIndex1 - 1;
		indices[j + 1] = temp.faces[i].vIndex2 - 1;
		indices[j + 2] = temp.faces[i].vIndex3 - 1;
		j += 3;
	}

	//indices[0] = 0; indices[1] = 1; indices[2] = 2; //Bottom
	//indices[3] = 1; indices[4] = 2; indices[5] = 3; //Bottom
	//indices[6] = 6; indices[7] = 7; indices[8] = 2; //Front
	//indices[9] = 7; indices[10] = 2; indices[11] = 3; //Front
	//indices[12] = 7; indices[13] = 5; indices[14] = 3; //Right
	//indices[15] = 5; indices[16] = 3; indices[17] = 1; //Right
	//indices[18] = 5; indices[19] = 4; indices[20] = 1; //Back
	//indices[21] = 4; indices[22] = 1; indices[23] = 0; //Back
	//indices[24] = 4; indices[25] = 6; indices[26] = 0; //Left
	//indices[27] = 6; indices[28] = 0; indices[29] = 2; //Left
	//indices[30] = 4; indices[31] = 5; indices[32] = 6; //Top
	//indices[33] = 5; indices[34] = 6; indices[35] = 7; //Top
}

void Graphics::SetVertices(const ObjectType& obj)
{
	for (int i = 0; i < nVertices; i++)
	{
		vertices[i] = obj.vertices[i];
		vertices[i].x += playerPosition.x;
		vertices[i].y += playerPosition.y;
		vertices[i].z += playerPosition.z;
	}
	for (int i = 0; i < nNormals; i++)
	{
		normals[i] = obj.normals[i];
	}

	/*vertices[0] = VertexType(position.x - size, position.y, position.z + size);
	vertices[1] = VertexType(position.x + size, position.y, position.z + size);
	vertices[2] = VertexType(position.x - size, position.y, position.z - size);
	vertices[3] = VertexType(position.x + size, position.y, position.z - size);
	vertices[4] = VertexType(position.x - size, position.y + size * 2, position.z + size);
	vertices[5] = VertexType(position.x + size, position.y + size * 2, position.z + size);
	vertices[6] = VertexType(position.x - size, position.y + size * 2, position.z - size);
	vertices[7] = VertexType(position.x + size, position.y + size * 2, position.z - size);*/
}

void Graphics::CreateBuffers()
{
	//Vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)*nVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = vertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	rDevice->CreateBuffer(&vertexBufferDesc, &vData, &vertexBuffer);

	//Index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT)*nIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iData;
	iData.pSysMem = indices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	rDevice->CreateBuffer(&indexBufferDesc, &iData, &indexBuffer);

	//Constant buffer
	D3D11_BUFFER_DESC cbObjectDesc;
	cbObjectDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbObjectDesc.ByteWidth = sizeof(constBufferPerObject);
	cbObjectDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbObjectDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbObjectDesc.MiscFlags = 0;
	cbObjectDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA cbData;
	cbData.pSysMem = &cbPerObject;
	cbData.SysMemPitch = 0;
	cbData.SysMemSlicePitch = 0;

	rDevice->CreateBuffer(&cbObjectDesc, &cbData, &cbPerObjectBuffer);
}

void Graphics::CreateCamera()
{
	camera->SetDistance();
	camera->SetRotation(Isometric);
	camera->SetFocus(game->getPlayerData().position);
	camera->Update(0.1f);

	world = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX view = camera->GetView();
	DirectX::XMMATRIX projection = camera->GetProjection();
	DirectX::XMMATRIX wvp = world * view * projection;
	DirectX::XMStoreFloat4x4(&cbPerObject.WVP, DirectX::XMMatrixTranspose(wvp));
}

HRESULT Graphics::Initialize(HWND &wndHandle, HINSTANCE &hInstance, int width, int height, float screenNear, float screenFar, bool fullscreen)
{
	HRESULT hr;

	size = 0.1f;
	nObjects = 1;
	nVertices = 8;
	nIndices = 36;

	hr = CreateDirect3DContext(wndHandle);
	if (FAILED(hr)) { return hr; }

	SetViewport(width, height);
	hr = CreateDepthBuffer(width, height);
	if (FAILED(hr)) { return hr; }

	hr = CreateShaders();
	if (FAILED(hr)) { return hr; }

	loader = new Loader();
	game = new GameDummy();
	camera = new Camera(Perspective, 1.0f, (float)width, (float)height, screenNear, screenFar);

	loader->Initialize(rDevice, 1);
	game->Initialize(wndHandle, hInstance, viewport);

	DirectX::XMStoreFloat3(&playerPosition, game->getPlayerData().position);
	CreateCamera();
	InitVertices();
	CreateBuffers();

	return hr;
}

void Graphics::Update()
{	
	game->Update();
	DirectX::XMStoreFloat3(&playerPosition, game->getPlayerData().position);
	SetVertices(loader->getObject(Player));
	world = game->getPlayerData().direction;
	camera->SetFocus(DirectX::XMVector3Transform(game->getPlayerData().position, world));
	camera->Update(0.1f);

	DirectX::XMMATRIX view = camera->GetView();
	DirectX::XMMATRIX projection = camera->GetProjection();
	DirectX::XMMATRIX wvp = world * view * projection;
	DirectX::XMStoreFloat4x4(&cbPerObject.WVP, DirectX::XMMatrixTranspose(wvp));
	DirectX::XMStoreFloat4x4(&cbPerObject.World, DirectX::XMMatrixTranspose(world));

	D3D11_MAPPED_SUBRESOURCE cb;
	ZeroMemory(&cb, sizeof(D3D11_MAPPED_SUBRESOURCE));
	rDeviceContext->Map(cbPerObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
	memcpy(cb.pData, &cbPerObject, sizeof(constBufferPerObject));
	rDeviceContext->Unmap(cbPerObjectBuffer, 0);

	D3D11_MAPPED_SUBRESOURCE vb;
	ZeroMemory(&vb, sizeof(D3D11_MAPPED_SUBRESOURCE));
	rDeviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vb);
	memcpy(vb.pData, vertices, sizeof(VertexType) * nVertices);
	rDeviceContext->Unmap(vertexBuffer, 0);
}

void Graphics::Render()
{
	float col[4] = { 0, 0, 1, 0 };
	rDeviceContext->ClearRenderTargetView(rBackbufferRTV, col);
	rDeviceContext->ClearDepthStencilView(rDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	rDeviceContext->IASetInputLayout(rVertexLayout);
	rDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	rDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, offset);
	rDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	rDeviceContext->VSSetShader(rVS, nullptr, 0);
	rDeviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	rDeviceContext->PSSetShader(rPS, nullptr, 0);

	rDeviceContext->DrawIndexed(nIndices, 0, 0);
}

void Graphics::SwapFBBuffer()
{
	rSwapChain->Present(0, 0);
}

void Graphics::ReleaseCOM()
{
	game->ReleaseCOM();

	if (rVertexLayout) { rVertexLayout->Release(); }
	if (rVS) { rVS->Release(); }
	if (rPS) { rPS->Release(); }

	vertexBuffer->Release();
	indexBuffer->Release();
	cbPerObjectBuffer->Release();

	rDepthStencilView->Release();
	rDepthStencilBuffer->Release();
	rBackbufferRTV->Release();
	rSwapChain->Release();
	rDevice->Release();
	rDeviceContext->Release();
}