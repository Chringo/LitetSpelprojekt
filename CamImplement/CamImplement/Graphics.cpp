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

	game = nullptr;
	camera = nullptr;
	dirLight = nullptr;

	cbPerFrameBuffer = nullptr;
	rVertexLayout = nullptr;
	rVS = nullptr;
	rPS = nullptr;

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

Graphics::Graphics(const Graphics &obj)
{

}

Graphics::~Graphics()
{
	delete game;
	delete camera;
	delete dirLight;
	delete pointLight;
	delete shadowMap;

	delete[] m_objArrowPosState;
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

	//Constant buffer
	D3D11_BUFFER_DESC cbObjectDesc;
	cbObjectDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbObjectDesc.ByteWidth = sizeof(constBufferPerObject);
	cbObjectDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbObjectDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbObjectDesc.MiscFlags = 0;
	cbObjectDesc.StructureByteStride = 0;

	rDevice->CreateBuffer(&cbObjectDesc, NULL, &cbPerObjectBuffer);

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
		rDevice->CreateBuffer(&vertexBufferDesc, &vData, &m_objPlayer->vertexBuffer);
		rDevice->CreateBuffer(&indexBufferDesc, &iData, &m_objPlayer->indexBuffer);
	}

	//Enemy buffer
	if (m_objEnemies)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objEnemies->nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objEnemies->nIndices;
		vData.pSysMem = m_objEnemies->input;
		iData.pSysMem = m_objEnemies->indices;
		rDevice->CreateBuffer(&vertexBufferDesc, &vData, &m_objEnemies->vertexBuffer);
		rDevice->CreateBuffer(&indexBufferDesc, &iData, &m_objEnemies->indexBuffer);
	}

	//Obstacle buffers
	if (m_objObstacles)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objObstacles->nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objObstacles->nIndices;
		vData.pSysMem = m_objObstacles->input;
		iData.pSysMem = m_objObstacles->indices;
		rDevice->CreateBuffer(&vertexBufferDesc, &vData, &m_objObstacles->vertexBuffer);
		rDevice->CreateBuffer(&indexBufferDesc, &iData, &m_objObstacles->indexBuffer);
	}

	//Map buffers
	if (m_objTiles)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objTiles->nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objTiles->nIndices;
		vData.pSysMem = m_objTiles->input;
		iData.pSysMem = m_objTiles->indices;
		rDevice->CreateBuffer(&vertexBufferDesc, &vData, &m_objTiles->vertexBuffer);
		rDevice->CreateBuffer(&indexBufferDesc, &iData, &m_objTiles->indexBuffer);
	}

	//Menu buffers
	if (m_objMenu)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objMenu->nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objMenu->nIndices;
		vData.pSysMem = m_objMenu->input;
		iData.pSysMem = m_objMenu->indices;
		rDevice->CreateBuffer(&vertexBufferDesc, &vData, &m_objMenu->vertexBuffer);
		rDevice->CreateBuffer(&indexBufferDesc, &iData, &m_objMenu->indexBuffer);
	}

	//Menu Arrow buffers
	if (m_objArrow)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objArrow->nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objArrow->nIndices;
		vData.pSysMem = m_objArrow->input;
		iData.pSysMem = m_objArrow->indices;
		rDevice->CreateBuffer(&vertexBufferDesc, &vData, &m_objArrow->vertexBuffer);
		rDevice->CreateBuffer(&indexBufferDesc, &iData, &m_objArrow->indexBuffer);
	}

	//Menu won buffers
	if (m_objWon)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objWon->nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objWon->nIndices;
		vData.pSysMem = m_objWon->input;
		iData.pSysMem = m_objWon->indices;
		rDevice->CreateBuffer(&vertexBufferDesc, &vData, &m_objWon->vertexBuffer);
		rDevice->CreateBuffer(&indexBufferDesc, &iData, &m_objWon->indexBuffer);
	}

	//Menu lost buffers
	if (m_objLost)
	{
		vertexBufferDesc.ByteWidth = sizeof(InputType) * m_objLost->nVertices;
		indexBufferDesc.ByteWidth = sizeof(UINT) * m_objLost->nIndices;
		vData.pSysMem = m_objLost->input;
		iData.pSysMem = m_objLost->indices;
		rDevice->CreateBuffer(&vertexBufferDesc, &vData, &m_objLost->vertexBuffer);
		rDevice->CreateBuffer(&indexBufferDesc, &iData, &m_objLost->indexBuffer);
	}
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

	game = new GameDummy();
	camera = new Camera(Perspective, 1.0f, (float)width, (float)height, screenNear, screenFar);
	dirLight = new DirectionalLight();
	pointLight = new PointLight();
	
	game->Initialize(wndHandle, hInstance, viewport);
	objInitialize(rDevice, game->GetEnemyArrSize(), game->GetObsArrSize(), game->GetNrOfTiles());
	renderMenu = gamePaused;
	SetTilesWorld(game->GetTileMatrices());
	SetObstaclesWorld(game->GetObsMatrices());
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
		IncreaseMenuState();
	}
	else if (KEYDOWN(VK_DOWN) && gamePaused)
	{
		DecreaseMenuState();
	}
	else if (KEYDOWN(VK_ESCAPE) && game->GetGameState())
	{
		gamePaused = !gamePaused;
		renderMenu = gamePaused;
	}
	else if (KEYDOWN(VK_RETURN))
	{
		if (gamePaused && renderMenu)
		{
			if (currentState == 0)
			{
				gamePaused = false;
				renderMenu = gamePaused;
				game->NewGame();
			}
			else
			{
				// Close the game
				return false;
			}
		}
		else if (renderWon)
		{
			renderWon = false;
			renderMenu = true;
		}
		else if (renderLost)
		{
			renderLost = false;
			renderMenu = true;
		}
	}

	if (game->GetGameState() != gOngoing && !gamePaused)
	{
		GameState asfd = game->GetGameState();
		gamePaused = true;
		if (game->GetGameState() == gWon)
		{
			renderWon = gamePaused;
		}
		else if (game->GetGameState() == gLost)
		{
			renderLost = gamePaused;
		}
	}

	/****************************************************************************************/

	if (!gamePaused)
	{
		game->Update(deltaTime);
	}

	camera->SetFocus(game->GetPlayerPosition());
	camera->Update(deltaTime);

	SetPlayerWorld(game->GetPlayerMatrix());
	SetEnemiesWorld(game->GetEnemyMatrices());

	SetPlayerHit(game->IsPlayerHit());
	for (int i = 0; i < game->GetEnemyArrSize(); i++)
	{
		SetEnemyHit(i, game->IsEnemyHit(i));
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
	RenderGeometry(rDeviceContext, shadowViewProjection);
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

	XMStoreFloat4x4(&m_view, camera->GetView());
	XMStoreFloat4x4(&m_projection, camera->GetProjection());
	
	//
	rDeviceContext->PSSetSamplers(0, 1, &samplerState);


	//IMPORTANT: If you put two planes in the same matrix pos,
	//the one that is called first (Arrow vs Menu for example)
	//gets rendered over the other one.
	RenderInstances(rDeviceContext, m_objPlayer);
	RenderInstances(rDeviceContext, m_objEnemies);
	RenderInstances(rDeviceContext, m_objObstacles);
	RenderInstances(rDeviceContext, m_objTiles);
	if (renderMenu)
	{
		RenderInstances(rDeviceContext, m_objArrow);
		RenderInstances(rDeviceContext, m_objMenu);
	}
	if (renderWon)
	{
		RenderInstances(rDeviceContext, m_objWon);
	}
	if (renderLost)
	{
		RenderInstances(rDeviceContext, m_objLost);
	}
}

void Graphics::SwapFBBuffer()
{
	rSwapChain->Present(0, 0);
}

void Graphics::ReleaseCOM()
{
	game->ReleaseCOM();
	
	//

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
		m_objMenu->Delete();
		delete m_objMenu;
	}
	if (m_objArrow)
	{
		m_objArrow->Delete();
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

	//

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








/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/

int framecount = 0;

void Graphics::InitInstances(Object obj, ObjectInstance *&object)
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

	for (int f = 0; f < nF; f++)
	{
		for (int j = 0; j < nI / 3; j++)
		{
			iV = temp->faces[j].vIndex1 - 1;
			object->fx.push_back(temp->frames[f].vertex[iV].x);
			object->fy.push_back(temp->frames[f].vertex[iV].y);
			object->fz.push_back(temp->frames[f].vertex[iV].z);

			iV = temp->faces[j].vIndex2 - 1;
			object->fx.push_back(temp->frames[f].vertex[iV].x);
			object->fy.push_back(temp->frames[f].vertex[iV].y);
			object->fz.push_back(temp->frames[f].vertex[iV].z);

			iV = temp->faces[j].vIndex3 - 1;
			object->fx.push_back(temp->frames[f].vertex[iV].x);
			object->fy.push_back(temp->frames[f].vertex[iV].y);
			object->fz.push_back(temp->frames[f].vertex[iV].z);
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

void Graphics::CreateSamplers(ID3D11Device* device)
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

void Graphics::RenderInstances(ID3D11DeviceContext* deviceContext, ObjectInstance* obj)
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
				obj->input[j].pos.x = obj->fx.at(j + framecount * (obj->nVertices));
				obj->input[j].pos.y = obj->fy.at(j + framecount * (obj->nVertices));
				obj->input[j].pos.z = obj->fz.at(j + framecount * (obj->nVertices));
			}
			framecount < 191 ? framecount++ : framecount = 0;


			D3D11_MAPPED_SUBRESOURCE vb;
			ZeroMemory(&vb, sizeof(D3D11_MAPPED_SUBRESOURCE));
			deviceContext->Map(obj->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vb);
			memcpy(vb.pData, obj->input, sizeof(InputType) * m_objPlayer->nVertices);
			deviceContext->Unmap(obj->vertexBuffer, 0);

			deviceContext->IASetVertexBuffers(0, 1, &obj->vertexBuffer, &stride, &offset);
			deviceContext->IASetIndexBuffer(obj->indexBuffer, DXGI_FORMAT_R32_UINT, offset);
		}

		// Pass data to shaders.
		deviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

		// Draw mesh.
		deviceContext->DrawIndexed(obj->nIndices, 0, 0);
	}
}

void Graphics::objInitialize(ID3D11Device* device, int nEnemies, int nObstacles, int nTiles)
{
	renderMenu = false;

	m_loader = new Loader();
	Object obj[] = { Player, Enemy, Obstacle, objMap, Menu, Arrow, Won, Lost };
	m_loader->Initialize(device, obj, (sizeof(obj) / sizeof(Object)));

	// Create meshes & buffers.
	InitInstances(Player, m_objPlayer);
	InitInstances(Enemy, m_objEnemies);
	InitInstances(Obstacle, m_objObstacles);
	InitInstances(objMap, m_objTiles);
	InitInstances(Menu, m_objMenu);
	InitInstances(Arrow, m_objArrow);
	InitInstances(Won, m_objWon);
	InitInstances(Lost, m_objLost);

	m_objArrowStateSize = 2;
	m_objArrowPosState = new DirectX::XMFLOAT2[m_objArrowStateSize];
	m_objArrowPosState[0] = DirectX::XMFLOAT2(0.5f, -0.7f);
	m_objArrowPosState[1] = DirectX::XMFLOAT2(0.3f, -0.9f);
	currentState = 1;

	CreateSamplers(device);

	// Create instances.
	XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, XMMatrixIdentity());

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
		m_objTiles->world.push_back(mat);

	m_objMenu->world.push_back(mat);

	m_objArrow->world.push_back(mat);

	m_objWon->world.push_back(mat);

	m_objLost->world.push_back(mat);

	XMStoreFloat4x4(&cbPerObject.World, XMMatrixIdentity());
	XMStoreFloat4x4(&cbPerObject.WVP, XMMatrixIdentity());
}

void Graphics::SetPlayerHit(bool hit)
{
	m_objPlayer->hit[0] = hit;
}

void Graphics::SetEnemyHit(int index, bool hit)
{
	m_objEnemies->hit[index] = hit;
}

/***************************************************************************/

void Graphics::SetPlayerWorld(const XMMATRIX &world)
{
	XMStoreFloat4x4(&m_objPlayer->world[0], world);
}

void Graphics::SetEnemiesWorld(const XMMATRIX* arr)
{
	for (UINT i = 0; i < m_objEnemies->world.size(); i++)
	{
		XMMATRIX w = arr[i];
		XMStoreFloat4x4(&m_objEnemies->world[i], w);
	}
}

void Graphics::SetObstaclesWorld(const XMMATRIX* arr)
{
	for (UINT i = 0; i < m_objObstacles->world.size(); i++)
	{
		//Passing world directly into StoreFloat causes random access violation
		XMMATRIX w = arr[i];
		XMStoreFloat4x4(&m_objObstacles->world[i], w);
	}
}

void Graphics::SetTilesWorld(const XMMATRIX* arr)
{
	for (UINT i = 0; i < m_objTiles->world.size(); i++)
	{
		//Passing world directly into StoreFloat causes random access violation
		XMMATRIX w = arr[i];
		XMStoreFloat4x4(&m_objTiles->world[i], w);
	}
}

/***************************************************************************/

void Graphics::IncreaseMenuState()
{
	currentState++;
	if (currentState > m_objArrowStateSize - 1)
	{
		currentState = 0;
	}
}

void Graphics::DecreaseMenuState()
{
	currentState--;
	if (currentState < 0)
	{
		currentState = m_objArrowStateSize - 1;
	}
}

void Graphics::SetAnimationState(int index, AnimationState animState)
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

void Graphics::RenderInstanceGeometry(ID3D11DeviceContext *deviceContext, ObjectInstance *object, const XMMATRIX &viewProjection)
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

void Graphics::RenderGeometry(ID3D11DeviceContext *deviceContext, const XMMATRIX &viewProjection)
{
	deviceContext->PSSetSamplers(1, 1, &pointSampler);

	// Required to convert calculate distance between light source and pixel.
	XMStoreFloat4x4(&m_shadowViewProjection, viewProjection);

	RenderInstanceGeometry(deviceContext, m_objPlayer, viewProjection);

	// [does not cast shadows properly]
	RenderInstanceGeometry(deviceContext, m_objEnemies, viewProjection);
	//RenderInstanceGeometry(deviceContext, m_objObstacles, viewProjection);

	// These have nothing to cast shadows on.
	//RenderInstanceGeometry(deviceContext, m_objTiles, viewProjection);
}
