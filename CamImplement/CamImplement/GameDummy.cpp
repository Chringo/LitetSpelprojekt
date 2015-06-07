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

	delete map;

	delete[] hitData[0];
	delete[] hitData[1];
}

void GameDummy::NewGame()
{
	gameState = gOngoing;

	/************************************* Player *************************************/

	if (player != nullptr)
	{
		delete player;
	}

	player = new Ent::Player(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), SCALE_MEDIUM);
	player->SetMovementSpeed(9.f);

	lastX = -1;
	lastZ = -1;

	/**********************************************************************************/
	/************************************* Enemy  *************************************/
	const DirectX::XMFLOAT4 blue = DirectX::XMFLOAT4(0.5f, 0.5f, 2.f, 1.f);
	const DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0.5f, 2.f, 0.5f, 1.f);
	const DirectX::XMFLOAT4 yellow = DirectX::XMFLOAT4(2.f, 2.f, 0.5f, 1.f);

	const XMFLOAT3 startpos = map->getBaseTiles()[0][0].worldpos;

	Ent::Enemy regular = Ent::Enemy(startpos, SCALE_MEDIUM, 7.f, 100.0f, 0.9f);
	Ent::Enemy runner = Ent::Enemy(startpos, SCALE_SMALL, 12.f, 80.0f, 0.6f, green);
	Ent::Enemy elite = Ent::Enemy(startpos, SCALE_MEDIUM, 8.f, 120.0f, 1.0f, yellow);
	Ent::Enemy giant = Ent::Enemy(startpos, SCALE_LARGE, 6.f, 200.0f, 1.3f, blue);

	if (enemyArr != nullptr)
	{
		delete[] lastEnemyCoord;
		for (size_t i = 0; i < (size_t)enemyArrSize; i++)
		{
			delete enemyArr[i];
		}
		delete[] enemyArr;
	}

	enemyArrSize = 3;
	lastEnemyCoord = new PF::Pathfinding::Coordinate[enemyArrSize];
	enemyMatrixArr = new XMMATRIX[enemyArrSize];
	hitData[0] = new bool[enemyArrSize];
	hitData[1] = new bool[enemyArrSize];
	enemyArr = new Ent::Enemy*[enemyArrSize];
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		lastEnemyCoord[i] = PF::Pathfinding::Coordinate(-1, -1);

		if (i == 0) enemyArr[i] = new Ent::Enemy(runner);
		else if (i == 1) enemyArr[i] = new Ent::Enemy(elite);
		else enemyArr[i] = new Ent::Enemy(giant);

		enemyArr[i]->SetPosition(map->getBaseTiles()[1][i * 10 + 3].worldpos);

		enemyMatrixArr[i] = XMMatrixIdentity();
		hitData[0][i] = false;
		hitData[1][i] = false;
	}
	/**********************************************************************************/
}

HRESULT GameDummy::Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport)
{
	RECT r;
	windowHandle = wndHandle;
	GetWindowRect(wndHandle, &r);
	clientSize.x = r.right - r.left;
	clientSize.y = r.bottom - r.top;

	/************************************** Map  **************************************/

	map = new Map(3, 5, 60.0f);

	/**********************************************************************************/
	/************************************ Obstacle ************************************/
	worldBounds = new Ent::Obstacle(62, 62, 1.f, 63, 63);

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
				obsArr[index] = new Ent::Obstacle(map->getBaseTiles()[h][w].worldpos.x, map->getBaseTiles()[h][w].worldpos.z, 5.f, 2.f, 2.f);
				obsMatrixArr[index] = XMMatrixIdentity();
				index++;
			}
		}
	}

	NewGame();

	return S_OK;
}

void GameDummy::CheckPlayerAttack()
{
	if (player->GetAttackValue() > 0)
	{
		for (size_t i = 0; i < (size_t)enemyArrSize; i++)
		{
			hitData[0][i] = false;
		}
	}
	else return;

	XMVECTOR atkPos = player->GetAttackPosition();
	float d = 0.f;
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		d = XMVector3Length(enemyArr[i]->GetPosition() - atkPos).m128_f32[0];
		if (d < 5.f && !hitData[0][i])
		{
			hitData[0][i] = true;
			enemyArr[i]->DecreaseHealth(player->GetAttackValue());
		}
	}
}

void GameDummy::CheckEnemyAttack(int index)
{
	Ent::Action action = enemyArr[index]->GetCurrentAction();
	int frame = enemyArr[index]->GetCurrentActionFrame();

	if (enemyArr[index]->GetAttackValue() > 0)
		hitData[1][index] = false;
	else return;

	XMVECTOR atkPos = enemyArr[index]->GetAttackPosition();
	float d = XMVector3LengthEst(player->GetPosition() - atkPos).m128_f32[0];
	if (d < 5.f && !hitData[1][index])
	{
		hitData[1][index] = true;
		player->DecreaseHealth(enemyArr[index]->GetAttackValue());
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

	size_t index = 0;
	while (index < (size_t)enemyArrSize)
	{
		if (enemyArr[index]->IsDead())
		{
			delete enemyArr[index];
			enemyArr[index] = enemyArr[enemyArrSize-1];
			enemyArrSize--;
		}
		else index++;
	}

	// player won
	if (enemyArrSize == 0)
	{
		gameState = gWon;
	}

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

	const float ts = map->TILESIZE;
	const int cs = map->getChunkSize();

	for (size_t i = 0; i < (size_t)obsArrSize; i++)
	{
		int x = obsArr[i]->getXTileSpace(ts);
		int z = obsArr[i]->getZTileSpace(ts);
		disable[x][z] = true;
	}

	bool update = false;

	// Block and update if an Enemy moves
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		disable[enemyArr[i]->getXTileSpace(ts, cs)][enemyArr[i]->getZTileSpace(ts, cs)] = true;
		PF::Pathfinding::Coordinate c(enemyArr[i]->getXTileSpace(ts, cs), enemyArr[i]->getZTileSpace(ts, cs));
		if (c != lastEnemyCoord[i])
		{
			lastEnemyCoord[i] = c;
			update = true;
		}
	}

	// Update if player moves
	if (lastX != player->getXTileSpace(ts, cs) || lastZ != player->getZTileSpace(ts, cs))
	{
		lastX = player->getXTileSpace(ts, cs);
		lastZ = player->getZTileSpace(ts, cs);
		update = true;
	}

	// Makes handling of A* easier. Deallocates the 2D bool array
	PF::Map* pfMap = new PF::Map(disable, map->getChunkSize());

	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
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

	/***************************************************************************************/
	/************************************** Collision **************************************/

	for (UINT obstacleId = 0; obstacleId < obsArrSize; obstacleId++)
		player->Intersect(obsArr[obstacleId]);
	player->Intersect(worldBounds);

	if (!player->IsDead())
	{
		player->Update(deltaTime);
		player->SetAttackDirection(cursor);
		CheckPlayerAttack();
	}
	else // player lose
	{
		gameState = gLost;
	}

	// Update game objects.
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		if (XMVector3LengthEst(player->GetPosition() - enemyArr[i]->GetPosition()).m128_f32[0] < 6.f)
		{
			enemyArr[i]->SetAttackDirection(player->GetPosition());
			enemyArr[i]->PerformAction(Ent::Attack1);
		}
		//CheckEnemyAttack(i);

		enemyArr[i]->Update(deltaTime);
		enemyArr[i]->Intersect(worldBounds);
		player->Intersect(enemyArr[i]);
		for (size_t j = i + 1; j < (size_t)enemyArrSize; j++)
		{
			if (i != j) enemyArr[i]->Intersect(enemyArr[j]);
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

XMFLOAT4 GameDummy::GetPlayerColor() const
{
	return player->GetColor();
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

DirectX::XMVECTOR GameDummy::GetEnemyPosition(int index)
{
	return enemyArr[index]->GetPosition();
}

DirectX::XMFLOAT4 GameDummy::GetEnemyColor(int index) const
{
	return enemyArr[index]->GetColor();
}

Ent::Action GameDummy::GetEnemyAction(int index)
{
	return enemyArr[index]->GetCurrentAction();
}

float GameDummy::GetEnemyHitPoints(int index)
{
	return enemyArr[index]->GetHitPoints();
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

DirectX::XMMATRIX GameDummy::GetMapMatrix() const
{
	return map->setMapPlane();
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

GameState GameDummy::GetGameState() const
{
	return gameState;
}

void GameDummy::ReleaseCOM(){}
