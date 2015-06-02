#include "GameDummy.h"

using namespace DirectX;

GameDummy::GameDummy()
{
	player = nullptr;

	lastEnemyCoord = nullptr;
	enemyArr = nullptr;
	enemyMatrixArr = nullptr;

	map = nullptr;
}

GameDummy::~GameDummy()
{
	delete player;

	delete[] lastEnemyCoord;
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

	/************************************** Map  **************************************/

	map = new Map(1, 5, 60.0f);
	tileMatrixArr = new XMMATRIX[GetNrOfTiles()];
	for (size_t i = 0; i < (size_t)GetNrOfTiles(); i++)
	{
		tileMatrixArr[i] = XMMatrixIdentity();
	}
	/**********************************************************************************/	
	/************************************* Player *************************************/

	player = new Ent::Player(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
	player->SetMovementSpeed(9.f);

	lastX = -1;
	lastZ = -1;

	/**********************************************************************************/	
	/************************************* Enemy  *************************************/

	enemyArrSize = 3;
	lastEnemyCoord = new PF::Pathfinding::Coordinate[enemyArrSize];
	enemyMatrixArr = new XMMATRIX[enemyArrSize];
	hitData = new bool[enemyArrSize];
	enemyArr = new Ent::Enemy*[enemyArrSize];
	for (int i = 0; i < enemyArrSize; i++)
	{
		lastEnemyCoord[i] = PF::Pathfinding::Coordinate(-1, -1);
		enemyArr[i] = new Ent::Enemy(map->getBaseTiles()[0][i + 3].worldpos);
		enemyArr[i]->SetMovementSpeed(8.f);
		enemyMatrixArr[i] = XMMatrixIdentity();
		hitData[i] = false;
	}
	/**********************************************************************************/

	return S_OK;
}

void GameDummy::CheckPlayerAttack()
{
	Ent::Action action = player->GetCurrentAction();
	int frame = player->GetCurrentActionFrame();

	if (frame == 50)
	{
		for (int i = 0; i < enemyArrSize; i++)
		{
			hitData[i] = false;
		}
	}
	if ((action != Ent::Attack1 && action != Ent::Attack2)
		|| (frame <= 20 || frame >= 40))
		return;

	XMVECTOR atkPos = player->GetAttackPosition();
	float d = 0.f;
	for (int i = 0; i < enemyArrSize; i++)
	{
		d = XMVector3Length(enemyArr[i]->GetPosition() - atkPos).m128_f32[0];
		if (d < 5.f && !hitData[i])
		{
			hitData[i] = true;
			enemyArr[i]->Attack();
		}
	}
}

void GameDummy::Update(float deltaTime)
{
	// Output (total average) fps.
	elapsedTime += deltaTime;
	frames++;

	std::wstringstream wss;
	wss << "FPS: " << (INT)((float)frames / elapsedTime) << " Timer: " << (INT)(elapsedTime);
	SetWindowText(windowHandle, wss.str().c_str());

	// Get cursor position.
	POINT cursor;
	GetCursorPos(&cursor);
	ScreenToClient(windowHandle, &cursor);

	// Adjust to client center.
	cursor.x -= (LONG)(clientSize.x * 0.5f - 8);
	cursor.y -= (LONG)(clientSize.y * 0.5f - 16);

	/************************************* Pathfinding *************************************/

	// Allocates 2D bool array used for marking tiles as blocked
	bool** disable = new bool*[map->getChunkSize()];
	for (int i = 0; i < map->getChunkSize(); i++)
	{
		disable[i] = new bool[map->getChunkSize()];
		for (int j = 0; j < map->getChunkSize(); j++)
		{
			disable[i][j] = false;
		}
	}

	float ts = map->TILESIZE;

	bool update = false;

	// Block and update if an Enemy moves
	for (int i = 0; i < enemyArrSize; i++)
	{
		//disable[enemyArr[i]->getXTileSpace(ts)][enemyArr[i]->getZTileSpace(ts)] = true;
		PF::Pathfinding::Coordinate c(enemyArr[i]->getXTileSpace(ts), enemyArr[i]->getZTileSpace(ts));
		if (c != lastEnemyCoord[i])
		{
			lastEnemyCoord[i] = c;
			update = true;
		}
	}

	// Update if player moves
	if (lastX != player->getXTileSpace(ts) || lastZ != player->getZTileSpace(ts))
	{
		lastX = player->getXTileSpace(ts);
		lastZ = player->getZTileSpace(ts);
		update = true;
	}

	// Makes handling of A* easier. Deallocates the 2D bool array
	PF::Map* pfMap = new PF::Map(disable, map->getChunkSize());

	for (int i = 0; i < enemyArrSize; i++)
	{
		// Update path if a player or Enemy have moved from a tile to another
		if (update)
		{
			enemyArr[i]->setPathfinding
			(
				map,
				pfMap,
				player->GetPosition().m128_f32[0],
				player->GetPosition().m128_f32[2]
			);
		}
		enemyArr[i]->updateMoveOrder();
	}

	delete pfMap;
	
	/************************************* Pathfinding *************************************/

	if (!player->IsDead())
	{
		player->Update(deltaTime);
		player->SetAttackDirection(cursor);
		CheckPlayerAttack();
	}
	
	// Update game objects.
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		bool dead = enemyArr[i]->IsDead();
		if (!dead)
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
}

XMMATRIX GameDummy::GetPlayerMatrix()
{
	return player->GetTransform();
}

XMVECTOR GameDummy::GetPlayerPosition()
{
	return player->GetPosition();
}

Ent::Action GameDummy::GetPlayerAction()
{
	Ent::Action a = player->GetCurrentAction();
	return a;
}

float GameDummy::GetPlayerHitPoints()
{
	return player->GetHitPoints();
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

DirectX::XMVECTOR GameDummy::GetEnemyPosition(int index)
{
	return enemyArr[index]->GetPosition();
}

Ent::Action GameDummy::GetEnemyAction(int index)
{
	Ent::Action a = enemyArr[index]->GetCurrentAction();
	return a;
}

float GameDummy::GetEnemyHitPoints(int index)
{
	return enemyArr[index]->GetHitPoints();
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

bool GameDummy::IsPlayerHit()
{
	return player->GetHitFrameCount() || player->IsDead();
}

bool GameDummy::IsEnemyHit(int index)
{
	return enemyArr[index]->GetHitFrameCount() || enemyArr[index]->IsDead();
}

void GameDummy::ReleaseCOM(){}
