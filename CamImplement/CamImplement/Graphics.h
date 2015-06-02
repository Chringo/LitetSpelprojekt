#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "ObjectManager.h"
#include "Camera.h"
#include "GameDummy.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "GUI.h"
#include <Windows.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#define NUMBER_OF_LIGHTS 1
#define KEYDOWN(vkey)	(GetAsyncKeyState(vkey) & 0x1)

class Graphics
{
private:
	struct constBufferPerFrame
	{
		DirLight dirLight;
		Light light;
	} cbPerFrame;

	ObjectManager*			objManager;
	GameDummy*				game;
	Camera*					camera;
	DirectionalLight*		dirLight;
	PointLight*				pointLight;
	GUI*					gui;

	ID3D11Buffer*			cbPerFrameBuffer;

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

private:
	HRESULT CreateDirect3DContext(HWND &wndHandle);
	void SetViewport(int width, int height);
	HRESULT CreateDepthBuffer(int width, int height);
	HRESULT CreateShaders();
	void CreateBuffers();
	void CreateCamera();
	bool gamePaused;

public:
	Graphics();
	Graphics(const Graphics &obj);
	~Graphics();

	HRESULT Initialize(HWND &wndHandle, HINSTANCE &hInstance, int width, int height, float screenNear, float screenFar, bool fullscreen);

	bool Update(float deltaTime);
	void Render();

	void SwapFBBuffer();
	void ReleaseCOM();
};

#endif