#include "GameDummy.h"

using namespace DirectX;

GameDummy::GameDummy()
{
	player = nullptr;
	map = nullptr;
}

GameDummy::~GameDummy()
{
	delete player;
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		delete enemyArr[i];
	}
	delete enemyArr;
	delete[] map;
	//delete[] tileMatrixArr;//TODO Make sure to deallocate tileMatrixArr properly
}

HRESULT GameDummy::Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport)
{
	RECT r;
	windowHandle = wndHandle;
	GetWindowRect(wndHandle, &r);
	clientSize.x = r.right - r.left;
	clientSize.y = r.bottom - r.top;

	map = new Map();

	player = new Collision::Player(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
	player->SetMovementSpeed(0.02f);

	enemyArrSize = 5;
	enemyMatrixArr = new XMMATRIX[enemyArrSize];
	enemyArr = new Collision::Enemy*[enemyArrSize];
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		enemyArr[i] = new Collision::Enemy(0, i+3);
		enemyMatrixArr[i] = XMMatrixIdentity();
	}

	return S_OK;
}

void GameDummy::Update()
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(windowHandle, &p);

	p.x -= clientSize.x * 0.5f;
	p.y -= clientSize.y * 0.5f;

	player->Update(0.1f);
	player->SetAttackDirection(p);
}

XMMATRIX GameDummy::GetPlayerMatrix()
{
	return player->GetTransform();
}

XMVECTOR GameDummy::GetPlayerPosition()
{
	return player->GetPosition();
}



/// 
/// Enemies
///
int GameDummy::GetEnemyArrSize()
{
	return enemyArrSize;
}
DirectX::XMMATRIX* GameDummy::GetEnemyMatrices()
{
	//XMMATRIX* arr = new XMMATRIX[enemyArrSize];
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		enemyMatrixArr[i] = enemyArr[i]->GetTransform();
	}
	return enemyMatrixArr;
}
DirectX::XMVECTOR* GameDummy::GetEnemyPositions()
{
	XMVECTOR* arr = new XMVECTOR[enemyArrSize];
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		arr[i] = enemyArr[i]->GetPosition();
	}
	return arr;
}
/// 
/// Enemies
///

DirectX::XMMATRIX* GameDummy::GetTileMatrices()
{
	tileMatrixArr = map->getTileMatrices();
	return tileMatrixArr;
}
int GameDummy::GetNrOfTiles() const
{
	return map->getNrOfTiles();
}

void GameDummy::ReleaseCOM()
{
}