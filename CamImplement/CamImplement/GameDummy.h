#ifndef GAMEDUMMY_H
#define GAMEDUMMY_H

#include <DirectXMath.h>
#include <sstream>

#include "Map.h"
#include "Entity.h"
#include "Input.h"
#include "LinkedList.h"

class GameDummy
{
private:
	Map* map;
	DirectX::XMMATRIX* tileMatrixArr;

	// Testing bed for pathfinding - TO BE REMOVED LATER
	const int tempMapSize = 10;
	DirectX::XMFLOAT3** tempMap;
	LinkedList<DirectX::XMFLOAT3> path;
	// 

	Collision::Player* player;
	
	int enemyArrSize;
	Collision::Enemy** enemyArr;
	DirectX::XMMATRIX* enemyMatrixArr;

	POINT clientSize;
	HWND windowHandle;
	float elapsedTime = 0.f;
	int frames = 0;

public:
	GameDummy();
	~GameDummy();

	HRESULT Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport);

	void Update(float deltaTime);

	DirectX::XMMATRIX GetPlayerMatrix();
	DirectX::XMVECTOR GetPlayerPosition();

	int GetEnemyArrSize();
	DirectX::XMMATRIX* GetEnemyMatrices();

	DirectX::XMMATRIX* GetTileMatrices();
	int GetNrOfTiles() const;

	void ReleaseCOM();
};

#endif