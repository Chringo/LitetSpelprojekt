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

	path = LinkedList<XMFLOAT3>();

	for (int x = 1; x < map->getChunkSize(); x++)
	{
		for (int z = 0; z < map->getChunkSize(); z++)
		{
			path.insertLast(map->getBaseTiles()[x][z].worldpos);
		}	
	}

	player = new Collision::Player(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
	player->SetMovementSpeed(4.f);

	enemyArrSize = 3;
	enemyMatrixArr = new XMMATRIX[enemyArrSize];
	enemyArr = new Collision::Enemy*[enemyArrSize];
	for (int i = 0; i < enemyArrSize; i++)
	{
<<<<<<< HEAD
		enemyArr[i] = new Collision::Enemy(map->getBaseTiles()[0][i+3].worldpos);
		enemyArr[i]->SetMovementSpeed(4.f);
=======
		enemyArr[i] = new Collision::Enemy(0, i * 5 + 3);
		enemyArr[i]->SetMovementSpeed(2.0f);
>>>>>>> origin/collision-v2
		enemyMatrixArr[i] = XMMatrixIdentity();
	}

	return S_OK;
}

void GameDummy::Update(float deltaTime)
{
	// Output fps.
	elapsedTime += deltaTime;
	frames++;

	std::wstringstream wss;
	wss << "Fps: " << (INT)((float)frames / elapsedTime);
	SetWindowText(windowHandle, wss.str().c_str());

	// Get cursor position.
	POINT cursor;
	GetCursorPos(&cursor);
	ScreenToClient(windowHandle, &cursor);

	// Adjust to client center.
	cursor.x -= (LONG)(clientSize.x * 0.5f - 8);
	cursor.y -= (LONG)(clientSize.y * 0.5f - 16);

	player->Update(deltaTime);
	player->SetAttackDirection(cursor);

	// Temporary example that should be removed later
	bool there = true;
	if (enemyArr[2]->GetPosition().m128_f32[0] < path.elementAt(0).x - 0.1f)
	{
<<<<<<< HEAD
		enemyArr[2]->enqueueAction(Collision::MoveRight);
=======
		enemyArr[2]->PerformAction(Collision::MoveRight);
>>>>>>> origin/collision-v2
		there = false;
	}
	else if (enemyArr[2]->GetPosition().m128_f32[0] > path.elementAt(0).x + 0.1f)
	{
<<<<<<< HEAD
		enemyArr[2]->enqueueAction(Collision::MoveLeft);
=======
		enemyArr[2]->PerformAction(Collision::MoveLeft);
>>>>>>> origin/collision-v2
		there = false;
	}

	if (enemyArr[2]->GetPosition().m128_f32[2] < path.elementAt(0).z - 0.1f)
	{
<<<<<<< HEAD
		enemyArr[2]->enqueueAction(Collision::MoveUp);
=======
		enemyArr[2]->PerformAction(Collision::MoveUp);
>>>>>>> origin/collision-v2
		there = false;
	}
	else if (enemyArr[2]->GetPosition().m128_f32[2] > path.elementAt(0).z + 0.1f)
	{
<<<<<<< HEAD
		enemyArr[2]->enqueueAction(Collision::MoveDown);
=======
		enemyArr[2]->PerformAction(Collision::MoveDown);
>>>>>>> origin/collision-v2
		there = false;
	}

	if (there)
	{
		path.insertLast(path.elementAt(0));
		path.removeFirst();
	}

	// Update enemies.
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		enemyArr[i]->Update(deltaTime);
		player->Intersect(enemyArr[i]);
		for (size_t j = i + 1; j < (size_t)enemyArrSize; j++)
		{
			if (i != j)
				enemyArr[i]->Intersect(enemyArr[j]);
		}
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
