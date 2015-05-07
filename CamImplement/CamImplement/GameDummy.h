#ifndef GAMEDUMMY_H
#define GAMEDUMMY_H

#include <DirectXMath.h>

#include "Map.h"
#include "Entity.h"
#include "Input.h"

class GameDummy
{
private:
	Map* map;

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

	DirectX::XMMATRIX* GetTileMatrices();

	void ReleaseCOM();
};
#endif