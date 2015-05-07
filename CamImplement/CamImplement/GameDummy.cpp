#include "GameDummy.h"

using namespace DirectX;

GameDummy::GameDummy()
{
	player = nullptr;
}

GameDummy::~GameDummy()
{
	delete player;
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		delete enemyArr[i];
	}
	delete enemyArr;
}

HRESULT GameDummy::Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport)
{
	RECT r;
	windowHandle = wndHandle;
	GetWindowRect(wndHandle, &r);
	clientSize.x = r.right - r.left;
	clientSize.y = r.bottom - r.top;

	player = new Collision::Player(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
	player->SetMovementSpeed(0.02f);

	enemyArrSize = 5;
	enemyArr = new Collision::Enemy*[enemyArrSize];
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		enemyArr[i] = new Collision::Enemy(0, i);
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
	XMMATRIX* arr = new XMMATRIX[enemyArrSize];
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		arr[i] = enemyArr[i]->GetTransform();
	}
	return arr;
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



void GameDummy::ReleaseCOM()
{
}