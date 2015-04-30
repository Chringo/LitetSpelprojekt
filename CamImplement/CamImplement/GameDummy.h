#ifndef GAMEDUMMY_H
#define GAMEDUMMY_H

#include <DirectXMath.h>

#include "Entity.h"
#include "Input.h"

class GameDummy
{
private:
	Collision::Player* player;
	POINT clientSize;
	HWND windowHandle;

public:
	GameDummy();
	~GameDummy();

	HRESULT Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport);

	void Update();

	DirectX::XMMATRIX GetPlayerMatrix();
	DirectX::XMVECTOR GetPlayerPosition();

	void ReleaseCOM();
};

#endif