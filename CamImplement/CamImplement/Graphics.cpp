#include "Graphics.h"

Graphics::Graphics()
{
	rSwapChain = nullptr;
	rDevice = nullptr;
	rDeviceContext = nullptr;
	rBackbufferRTV = nullptr;
	rDepthStencilView = nullptr;
	rDepthStencilBuffer = nullptr;

	objManager = nullptr;
	game = nullptr;
	camera = nullptr;
	dirLight = nullptr;

	cbPerFrameBuffer = nullptr;
	rVertexLayout = nullptr;
	rVS = nullptr;
	rPS = nullptr;
}

Graphics::Graphics(const Graphics &obj)
{

}

Graphics::~Graphics()
{
	delete objManager;
	delete game;
	delete camera;
	delete dirLight;
	delete pointLight;
	delete gui;
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

	int deviceFlag = 0;
#ifdef _DEBUG
	deviceFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		deviceFlag,
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

void Graphics::CreateBuffers()
{
	//Constant buffer
	D3D11_BUFFER_DESC cbFrameDesc;
	cbFrameDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbFrameDesc.ByteWidth = sizeof(DirLight) + (sizeof(Light));// * NUMBER_OF_LIGHTS);
	cbFrameDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbFrameDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbFrameDesc.MiscFlags = 0;
	cbFrameDesc.StructureByteStride = 0;

	//cbPerFrame.light = new Light[NUMBER_OF_LIGHTS];

	D3D11_SUBRESOURCE_DATA cbData;
	cbData.pSysMem = &cbPerFrame;
	cbData.SysMemPitch = 0;
	cbData.SysMemSlicePitch = 0;

	rDevice->CreateBuffer(&cbFrameDesc, &cbData, &cbPerFrameBuffer);
}

void Graphics::CreateCamera()
{
	camera->SetDistance();
	camera->SetRotation(Isometric);
	camera->SetFocus(game->GetPlayerPosition());
	camera->Update(0.1f);
}

HRESULT Graphics::Initialize(HWND &wndHandle, HINSTANCE &hInstance, int width, int height, float screenNear, float screenFar, bool fullscreen)
{
	HRESULT hr;

	hr = CreateDirect3DContext(wndHandle);
	if (FAILED(hr)) { return hr; }

	SetViewport(width, height);
	hr = CreateDepthBuffer(width, height);
	if (FAILED(hr)) { return hr; }

	hr = CreateShaders();
	if (FAILED(hr)) { return hr; }

	objManager = new ObjectManager();
	game = new GameDummy();
	camera = new Camera(Perspective, 1.0f, (float)width, (float)height, screenNear, screenFar);
	//camera = new Camera(Orthographic, 1.0f, 40, 24, 0.1f, 100.f);		(projektionsalternativ..)
	dirLight = new DirectionalLight();
	pointLight = new PointLight();
	gui = new GUI();

	game->Initialize(wndHandle, hInstance, viewport);
	objManager->Initialize(rDevice, game->GetEnemyArrSize(), 0, game->GetNrOfTiles());
	objManager->SetTilesWorld(game->GetTileMatrices());
	dirLight->Initialize(DIRLIGHT_DEFAULT_DIRECTION, DIRLIGHT_DEFAULT_AMBIENT, DIRLIGHT_DEFAULT_DIFFUSE);
	pointLight->Initialize(NUMBER_OF_LIGHTS);
	gui->Initialize();
	
	cbPerFrame.dirLight = dirLight->getLight();

	CreateCamera();
	CreateBuffers();

	return hr;
}

void Graphics::Update(float deltaTime)
{	
	game->Update(deltaTime);

	camera->SetFocus(game->GetPlayerPosition());
	camera->Update(deltaTime);

	objManager->SetPlayerWorld(game->GetPlayerMatrix());
	objManager->SetEnemiesWorld(game->GetEnemyMatrices());
	objManager->Update();
	objManager->setViewProjection(camera->GetView(), camera->GetProjection());

	pointLight->setPosition(0, game->GetPlayerPosition());
	pointLight->setColor(0, game->GetPlayerAction());
	cbPerFrame.light = pointLight->getLight(0);
	/*for (int i = 1; i < NUMBER_OF_LIGHTS; i++)
	{
		pointLight->setPosition(i, DirectX::XMVectorSet(i * 50.0f, 0.0f, i * 50.0f, 0.0f));
		cbPerFrame.light[i] = pointLight->getLight(i);
	}*/

	D3D11_MAPPED_SUBRESOURCE cb;
	ZeroMemory(&cb, sizeof(D3D11_MAPPED_SUBRESOURCE));
	rDeviceContext->Map(cbPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
	memcpy(cb.pData, &cbPerFrame, (sizeof(DirLight) + (sizeof(Light))));// * NUMBER_OF_LIGHTS)));
	rDeviceContext->Unmap(cbPerFrameBuffer, 0);
}

void Graphics::Render()
{
	float col[4] = { 0, 0, 0, 0 };
	rDeviceContext->ClearRenderTargetView(rBackbufferRTV, col);
	rDeviceContext->ClearDepthStencilView(rDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	rDeviceContext->IASetInputLayout(rVertexLayout);
	rDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	rDeviceContext->VSSetShader(rVS, nullptr, 0);
	rDeviceContext->PSSetShader(rPS, nullptr, 0);
	rDeviceContext->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer);

	objManager->Render (rDeviceContext);
	gui->Render();
}

void Graphics::SwapFBBuffer()
{
	rSwapChain->Present(0, 0);
}

void Graphics::ReleaseCOM()
{
	game->ReleaseCOM();
	objManager->ReleaseCOM();
	gui->ReleaseCOM();

	if (cbPerFrameBuffer) { cbPerFrameBuffer->Release(); }
	if (rVertexLayout) { rVertexLayout->Release(); }
	if (rVS) { rVS->Release(); }
	if (rPS) { rPS->Release(); }

	rDepthStencilView->Release();
	rDepthStencilBuffer->Release();
	rBackbufferRTV->Release();
	rSwapChain->Release();
	rDevice->Release();
	rDeviceContext->Release();
}