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
	/************************************ Obstacle ************************************/
	obsArrSize = map->getObstacles();
	obsMatrixArr = new XMMATRIX[obsArrSize];
	obsArr = new Ent::Obstacle*[obsArrSize];
	int cSize = map->getChunkSize();
	int index = 0;
	for (int h = 0; h < cSize; h++)
	{
		for (int w = 0; w < cSize; w++)
		{
			if (map->getBaseTiles()[h][w].obstacle)
			{
				obsArr[index] = new Ent::Obstacle(map->getBaseTiles()[h][w].worldpos.x, map->getBaseTiles()[h][w].worldpos.z, 5.f, 1.f, 1.f);
				obsMatrixArr[index] = XMMatrixIdentity();
				index++;
			}
		}
	}
	/**********************************************************************************/
	/************************************* Player *************************************/

	player = new Ent::Player(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
	player->SetMovementSpeed(4.f);

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
		enemyArr[i]->SetMovementSpeed(4.f);
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

	if ((action != Ent::Attack1 && action != Ent::Attack2)
		|| (frame <= 20 || frame >= 40))
		return;
	else if ((action == Ent::Attack1 || action == Ent::Attack2)
		&& frame == 50)
	{
		for (int i = 0; i < enemyArrSize; i++)
		{
			hitData[i] = false;
		}
		return;
	}

	XMVECTOR atkPos = player->GetAttackPosition();
	float d = 0.f;
	for (int i = 0; i < enemyArrSize; i++)
	{
		d = XMVector3LengthEst(enemyArr[i]->GetPosition() - atkPos).m128_f32[0];
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

	// Update game objects.
	player->Update(deltaTime);
	player->SetAttackDirection(cursor);
	CheckPlayerAttack();

	for (UINT obstacleId = 0; obstacleId < obsArrSize; obstacleId++)
		player->Intersect(obsArr[obstacleId]);

	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		enemyArr[i]->Update(deltaTime);

		//for (UINT obstacleId = 0; obstacleId < obsArrSize; obstacleId++)
			//enemyArr[i]->Intersect(obsArr[obstacleId]);
		
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

Ent::Action GameDummy::GetPlayerAction()
{
	return player->GetCurrentAction();
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
/// 
/// Enemies
///

///
/// Obstacles
///
DirectX::XMMATRIX* GameDummy::GetObsMatrices()
{
	for (size_t i = 0; i < (size_t)obsArrSize; i++)
	{
		obsMatrixArr[i] = obsArr[i]->GetTransform();
	}
	return obsMatrixArr;
}
int GameDummy::GetObsArrSize() const
{
	return this->obsArrSize;
}
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
