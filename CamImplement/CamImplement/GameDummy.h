#ifndef GAMEDUMMY_H
#define GAMEDUMMY_H

#include <DirectXMath.h>
//#include <vector.h>

#include "Entity.h"
#include "Input.h"

class GameDummy
{
private:
	Collision::Player* player;
	
	int enemyArrSize;
	Collision::Enemy** enemyArr;

	POINT clientSize;
	HWND windowHandle;

public:
	GameDummy();
	~GameDummy();

	HRESULT Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport);

	void Update();

	DirectX::XMMATRIX GetPlayerMatrix();
	DirectX::XMVECTOR GetPlayerPosition();

	int GetEnemyArrSize();
	DirectX::XMMATRIX* GetEnemyMatrices();
	DirectX::XMVECTOR* GetEnemyPositions();

	void ReleaseCOM();
};

#endif