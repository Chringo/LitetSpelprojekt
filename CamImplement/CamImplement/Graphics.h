#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Camera.h"
#include "GameDummy.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "GUI.h"
#include <Windows.h>
#include "ShadowMap.h"
#include <vector>
#include "Loader.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#define MAX_NUMBER_OF_LIGHTS 10
#define KEYDOWN(vkey)	(GetAsyncKeyState(vkey) & 0x1)

enum AnimationState
{
	Attack,
	WalkStart,
	WalkLoop,
	WalkEnd,
	AnBlock,
	AnDodge
};

#define HUE_DEFAULT	DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f)
#define HUE_HIT		DirectX::XMFLOAT4(2.f, 0.5f, 0.5f, 1.f)

struct InputType
{
	InputType() {}
	InputType(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz) : pos(x, y, z), tex(u, v), nor(nx, ny, nz) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tex;
	DirectX::XMFLOAT3 nor;
};

struct ObjectInstance
{
	ID3D11Buffer*		vertexBuffer;
	ID3D11Buffer*		indexBuffer;

	std::vector<DirectX::XMFLOAT4X4> world;
	std::vector<bool>	hit;

	InputType*			input;
	UINT*				indices;

	int					nVertices;
	int					nIndices;
	int					nNormals;
	int					textureIndex;
	int					frameCount;

	std::vector<float> fx, fy, fz;

	void Delete()
	{
		if (vertexBuffer) { vertexBuffer->Release(); }
		if (indexBuffer) { indexBuffer->Release(); }
		delete[] input;
		delete[] indices;
	}
};

class Graphics
{
private:
	struct constBufferPerFrame
	{
		DirLight dirLight;
		int nLights;
		float pad1, pad2, pad3;
	} cbPerFrame;

	struct constBufferPointLight
	{
		Light light[MAX_NUMBER_OF_LIGHTS];
	} cbPointLight;

	struct constBufferPerObject
	{
		DirectX::XMFLOAT4X4 WVP;
		DirectX::XMFLOAT4X4 World;
		DirectX::XMFLOAT4X4 ShadowWVP;
		DirectX::XMFLOAT4	Hue;
	}					cbPerObject;

	GameDummy*				game;
	Camera*					camera;
	DirectionalLight*		dirLight;
	PointLight*				pointLight;

	ID3D11Buffer*			cbPerFrameBuffer;
	ID3D11Buffer*			cbPointLightBuffer;

	ID3D11InputLayout*		rVertexLayout;
	ID3D11VertexShader*		rVS;
	ID3D11PixelShader*		rPS;

	D3D11_VIEWPORT			viewport;
	IDXGISwapChain*			rSwapChain;
	ID3D11Device*			rDevice;
	ID3D11DeviceContext*	rDeviceContext;
	ID3D11RenderTargetView* rBackbufferRTV;
	ID3D11DepthStencilView* rDepthStencilView;
	ID3D11Texture2D*		rDepthStencilBuffer;
	ID3D11RasterizerState*	rRasterizerState;

	ShadowMap*				shadowMap;
	DirectX::XMMATRIX		shadowViewProjection;
	
	bool gamePaused;

	DirectX::XMFLOAT4X4 m_view;
	DirectX::XMFLOAT4X4 m_projection;
	DirectX::XMFLOAT4X4 m_shadowViewProjection;

	Loader*				m_loader;

	ObjectInstance*		m_objPlayer;
	ObjectInstance*		m_objEnemies;
	ObjectInstance*		m_objObstacles;
	ObjectInstance*		m_objTiles;

	// GUI
	ObjectInstance*		m_objMenu;
	ObjectInstance*		m_objArrow;
	int					m_objArrowStateSize;
	int					currentState;
	DirectX::XMFLOAT2*	m_objArrowPosState;
	bool renderMenu;

	ObjectInstance*		m_objWon;
	bool renderWon;

	ObjectInstance*		m_objLost;
	bool renderLost;
	//

	ID3D11Buffer*		cbPerObjectBuffer;
	ID3D11SamplerState* samplerState;
	ID3D11SamplerState* pointSampler;
	

private:
	HRESULT CreateDirect3DContext(HWND &wndHandle);
	void CreateViewport(int width, int height);
	HRESULT CreateDepthBuffer(int width, int height);
	HRESULT CreateShaders();
	void CreateBuffers();
	void CreateCamera();	

	void InitInstances(Object obj, ObjectInstance *&object);
	bool LoadTextures(ID3D11Device* device);
	void RenderInstances(ID3D11DeviceContext* deviceContext, ObjectInstance* obj);
	void RenderInstanceGeometry(ID3D11DeviceContext* deviceContext, ObjectInstance *object, const DirectX::XMMATRIX &viewProjection);
	void CreateSamplers(ID3D11Device* device);

	/*********************************Object manager**********************************/

	void objInitialize(ID3D11Device* device, int nEnemies, int nObstacles, int nTiles);

	void SetPlayerHit(bool hit);
	void SetEnemyHit(int index, bool hit);

	void SetPlayerWorld(const DirectX::XMMATRIX &world);
	void SetEnemiesWorld(const DirectX::XMMATRIX* arr);
	void SetObstaclesWorld(const DirectX::XMMATRIX* arr);
	void SetTilesWorld(const DirectX::XMMATRIX* arr);
	void SetGUIWorld(const DirectX::XMMATRIX &world);

	// GUI
	void IncreaseMenuState();
	void DecreaseMenuState();
	//

	void SetAnimationState(int index, AnimationState animState);

	void RenderGeometry(ID3D11DeviceContext* deviceContext, const DirectX::XMMATRIX &viewProjection);

	void setViewProjection(const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &projection);

public:
	Graphics();
	Graphics(const Graphics &obj);
	~Graphics();

	HRESULT Initialize(HWND &wndHandle, HINSTANCE &hInstance, int width, int height, float screenNear, float screenFar, bool fullscreen);

	bool Update(float deltaTime);
	void CreateShadowMap();
	void Render();

	void SwapFBBuffer();
	void ReleaseCOM();
};



#endif