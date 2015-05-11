#include "GameDummy.h"

using namespace DirectX;

GameDummy::GameDummy()
{
	player = nullptr;
	enemyArr = nullptr;
	enemyMatrixArr = nullptr;
	map = nullptr;
}

GameDummy::~GameDummy()
{
	delete player;
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		delete enemyArr[i];
	}
	delete[] enemyArr;
	delete[] map;
}

HRESULT GameDummy::Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport)
{
	RECT r;
	windowHandle = wndHandle;
	GetWindowRect(wndHandle, &r);
	clientSize.x = r.right - r.left;
	clientSize.y = r.bottom - r.top;

	map = new Map();
	tileMatrixArr = new XMMATRIX[GetNrOfTiles()];
	for (size_t i = 0; i < (size_t)GetNrOfTiles(); i++)
	{
		tileMatrixArr[i] = XMMatrixIdentity();
	}

	player = new Collision::Player(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
	player->SetMovementSpeed(0.02f);

	enemyArrSize = 5;
	enemyMatrixArr = new XMMATRIX[enemyArrSize];
	enemyArr = new Collision::Enemy*[enemyArrSize];
	for (int i = 0; i < enemyArrSize; i++)
	{
		enemyArr[i] = new Collision::Enemy(0, i * 3);
		enemyArr[i]->SetMovementSpeed(0.02f);
		enemyMatrixArr[i] = XMMatrixIdentity();
	}

	return S_OK;
}

void GameDummy::Update()
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(windowHandle, &p);

	p.x -= clientSize.x * (LONG)0.5f;
	p.y -= clientSize.y * (LONG)0.5f;

	player->Update(0.1f);
	player->SetAttackDirection(p);

	// Temporary example that should be removed later
	enemyArr[2]->setAction(Collision::MoveRight);
	
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		enemyArr[i]->Update(0.1f);
	}
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
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		enemyMatrixArr[i] = enemyArr[i]->GetTransform();
	}
	return enemyMatrixArr;
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

void GameDummy::ReleaseCOM(){}
