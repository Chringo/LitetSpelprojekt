#ifndef GAMEDUMMY_H
#define GAMEDUMMY_H

#include "PlayerDummy.h"
#include "Input.h"

class GameDummy
{
private:
	Input* input;
	PlayerDummy* player;
public:
	GameDummy();
	~GameDummy();

	HRESULT Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport);

	void Update();
	PlayerWorld getPlayerData();

	void ReleaseCOM();
};

#endif