#include "Graphics.h"

using namespace DirectX;

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
	delete shadowMap;
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
	}
	else
	{
		MessageBox(0, L"Couldn't create device context", L"Error", MB_OK);
	}

	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = FALSE;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.f;
	rd.SlopeScaledDepthBias = 0.f;
	rd.DepthClipEnable = TRUE;
	rd.ScissorEnable = FALSE;
	rd.MultisampleEnable = FALSE;
	rd.AntialiasedLineEnable = FALSE;
	if (FAILED(hr = rDevice->CreateRasterizerState(&rd, &rRasterizerState)))
		return hr;

	return hr;
}

void Graphics::CreateViewport(int width, int height)
{
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
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
	cbFrameDesc.ByteWidth = sizeof(constBufferPerFrame);
	cbFrameDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbFrameDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbFrameDesc.MiscFlags = 0;
	cbFrameDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA cbData;
	cbData.pSysMem = &cbPerFrame;
	cbData.SysMemPitch = 0;
	cbData.SysMemSlicePitch = 0;
	rDevice->CreateBuffer(&cbFrameDesc, &cbData, &cbPerFrameBuffer);

	//Point light buffer
	cbFrameDesc.ByteWidth = sizeof(Light) * MAX_NUMBER_OF_LIGHTS;
	cbData.pSysMem = &cbPointLight;
	rDevice->CreateBuffer(&cbFrameDesc, &cbData, &cbPointLightBuffer);
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

	CreateViewport(width, height);
	hr = CreateDepthBuffer(width, height);
	if (FAILED(hr)) { return hr; }

	hr = CreateShaders();
	if (FAILED(hr)) { return hr; }

	gamePaused = false;

	objManager = new ObjectManager();
	game = new GameDummy();
	camera = new Camera(Perspective, 1.0f, (float)width, (float)height, screenNear, screenFar);
	dirLight = new DirectionalLight();
	pointLight = new PointLight();
	
	game->Initialize(wndHandle, hInstance, viewport);
	objManager->Initialize(rDevice, game->GetEnemyArrSize(), game->GetObsArrSize(), game->GetNrOfTiles());
	objManager->SetRenderMenu(gamePaused);
	objManager->SetTilesWorld(game->GetTileMatrices());
	objManager->SetObstaclesWorld(game->GetObsMatrices());
	dirLight->Initialize(DIRLIGHT_DEFAULT_DIRECTION, DIRLIGHT_DEFAULT_AMBIENT, DIRLIGHT_DEFAULT_DIFFUSE);
	cbPerFrame.dirLight = dirLight->getLight();
	cbPerFrame.nLights = 1 + game->GetEnemyArrSize();
	pointLight->Initialize(cbPerFrame.nLights);

	shadowMap = new ShadowMap();
	if (FAILED(hr = shadowMap->Initialize(rDevice, 8 * 1024, 8 * 1024)))
		return hr;

	CreateCamera();
	CreateBuffers();

	// These will remain static. 
	rDeviceContext->IASetInputLayout(rVertexLayout);
	rDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return hr;
}

bool Graphics::Update(float deltaTime)
{	
	/********************************** Gamestate handling **********************************/
	if (KEYDOWN(VK_UP) && gamePaused)
	{
		objManager->IncreaseMenuState();
	}
	else if (KEYDOWN(VK_DOWN) && gamePaused)
	{
		objManager->DecreaseMenuState();
	}
	else if (KEYDOWN(VK_ESCAPE))
	{
		gamePaused = !gamePaused;
		objManager->SetRenderMenu(gamePaused);
	}
	else if (KEYDOWN(VK_RETURN))
	{
		if (gamePaused && objManager->GetRenderMenu())
		{
			if (objManager->GetMenuState() == 0)
			{
				gamePaused = false;
				objManager->SetRenderMenu(gamePaused);
				game->NewGame();
			}
			else
			{
				// Close the game
				return false;
			}
		}
		else if (objManager->GetRenderWon())
		{
			objManager->SetRenderWon(false);
			objManager->SetRenderMenu(true);
		}
		else if (objManager->GetRenderLost())
		{
			objManager->SetRenderLost(false);
			objManager->SetRenderMenu(true);
		}
	}

	if (game->GetGameState() != gOngoing && !gamePaused)
	{
		GameState asfd = game->GetGameState();
		gamePaused = true;
		if (game->GetGameState() == gWon)
		{
			objManager->SetRenderWon(gamePaused);
		}
		else if (game->GetGameState() == gLost)
		{
			objManager->SetRenderLost(gamePaused);
		}
	}

	/****************************************************************************************/

	if (!gamePaused)
	{
		game->Update(deltaTime);
	}

	camera->SetFocus(game->GetPlayerPosition());
	camera->Update(deltaTime);

	objManager->SetPlayerWorld(game->GetPlayerMatrix());
	objManager->SetEnemiesWorld(game->GetEnemyMatrices());
	objManager->Update();// Dumb key events

	objManager->SetPlayerHit(game->IsPlayerHit());
	for (int i = 0; i < objManager->GetEnemyCount(); i++)
	{
		objManager->SetEnemyHit(i, game->IsEnemyHit(i));
	}

	pointLight->setPosition(0, game->GetPlayerPosition());
	pointLight->setColor(0, game->GetPlayerAction());
	pointLight->setRangeByHitPoints(0, game->GetPlayerHitPoints());
	cbPointLight.light[0] = pointLight->getLight(0);

	for (int i = 1; i < cbPerFrame.nLights; i++)
	{
		//Enemy array is not aligned with point light array, thus index (i) in light is index (i-1) in enemy
		pointLight->setPosition(i, game->GetEnemyPosition(i - 1));
		pointLight->setColor(i, game->GetEnemyAction(i - 1));
		pointLight->setRangeByHitPoints(i, game->GetEnemyHitPoints(i - 1));
		cbPointLight.light[i] = pointLight->getLight(i);
	}

	D3D11_MAPPED_SUBRESOURCE cb;
	ZeroMemory(&cb, sizeof(D3D11_MAPPED_SUBRESOURCE));
	rDeviceContext->Map(cbPointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
	memcpy(cb.pData, &cbPointLight, (sizeof(Light) * MAX_NUMBER_OF_LIGHTS));
	rDeviceContext->Unmap(cbPointLightBuffer, 0);

	return true;
}

void Graphics::CreateShadowMap()
{
	// Unbind depth texture from pipeline.
	ID3D11ShaderResourceView *nullSrv[1] = { nullptr };
	rDeviceContext->PSSetShaderResources(1, 1, nullSrv);

	// Set render target to depth view only.
	shadowMap->Apply(rDeviceContext);

	// Render scene depth from light point of view.
	XMFLOAT3 dir = dirLight->getLight().dir;
	XMVECTOR lightDir = XMLoadFloat3(&dir);
	shadowViewProjection = shadowMap->CreateViewProjection(game->GetPlayerPosition(), lightDir, 10.f);

	// Dont use pixel shader. We only need SV_POSITION.
	rDeviceContext->VSSetShader(rVS, nullptr, 0);
	rDeviceContext->PSSetShader(nullptr, nullptr, 0);

	// Draw geometry. All we need.
	objManager->RenderGeometry(rDeviceContext, shadowViewProjection);
}

void Graphics::Render()
{
	// Create shadow map.
	CreateShadowMap();
	
	// Draw scene using shadow map data.
	float col[4] = { 0, 0, 0, 0 };
	rDeviceContext->ClearRenderTargetView(rBackbufferRTV, col);
	rDeviceContext->ClearDepthStencilView(rDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	rDeviceContext->OMSetRenderTargets(1, &rBackbufferRTV, rDepthStencilView);
	rDeviceContext->RSSetViewports(1, &viewport);
	rDeviceContext->RSSetState(rRasterizerState);

	rDeviceContext->VSSetShader(rVS, nullptr, 0);
	rDeviceContext->PSSetShader(rPS, nullptr, 0);

	rDeviceContext->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer);
	rDeviceContext->PSSetConstantBuffers(1, 1, &cbPointLightBuffer);
	rDeviceContext->PSSetShaderResources(1, 1, shadowMap->GetDepthAsTexture());

	objManager->setViewProjection(camera->GetView(), camera->GetProjection());
	objManager->Render(rDeviceContext);
}

void Graphics::SwapFBBuffer()
{
	rSwapChain->Present(0, 0);
}

void Graphics::ReleaseCOM()
{
	game->ReleaseCOM();
	objManager->ReleaseCOM();

	if (cbPerFrameBuffer) { cbPerFrameBuffer->Release(); }
	if (cbPointLightBuffer){ cbPointLightBuffer->Release(); }
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