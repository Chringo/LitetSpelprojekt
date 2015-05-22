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
	player->SetMovementSpeed(4.f);

	enemyArrSize = 3;
	enemyMatrixArr = new XMMATRIX[enemyArrSize];
	enemyArr = new Collision::Enemy*[enemyArrSize];
	for (int i = 0; i < enemyArrSize; i++)
	{
		enemyArr[i] = new Collision::Enemy(map->getBaseTiles()[0][i+3].worldpos);
		enemyArr[i]->SetMovementSpeed(4.f);
		enemyMatrixArr[i] = XMMatrixIdentity();
	}

	PFTest();

	return S_OK;
}

void GameDummy::PFTest()
{
	int mAmount = map->getChunkSize();
	bool** disable = new bool*[mAmount];
	for (int i = 0; i < mAmount; i++)
	{
		disable[i] = new bool[mAmount];
		for (int j = 0; j < mAmount; j++)
		{
			disable[i][j] = false;
		}
	}

	int xs = (int)enemyArr[2]->GetPosition().m128_f32[0] / map->TILESIZE;
	int zs = (int)enemyArr[2]->GetPosition().m128_f32[2] / map->TILESIZE;
	PF::Pathfinding::Coordinate start = PF::Pathfinding::Coordinate(xs, zs);

	int xg = player->GetPosition().m128_f32[0] / map->TILESIZE;
	int zg = player->GetPosition().m128_f32[2] / map->TILESIZE;
	PF::Pathfinding::Coordinate goal = PF::Pathfinding::Coordinate(xg, zg);

	PF::Map pfMap = PF::Map(disable, mAmount);
	LinkedList<PF::Pathfinding::Coordinate> aPath = PF::Pathfinding::Astar(start, goal, pfMap);

	for (int i = 0; i < aPath.size(); i++)
	{
		PF::Pathfinding::Coordinate c = aPath.elementAt(i);
		path.insertLast(map->getBaseTiles()[c.x][c.z].worldpos);
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

	// Temporary example that should be removed later
	if (path.size() > 0)
	{
		bool there = true;
		if (enemyArr[2]->GetPosition().m128_f32[0] < path.elementAt(0).x - 0.1f)
		{
			enemyArr[2]->enqueueAction(Collision::MoveRight);
			there = false;
		}
		else if (enemyArr[2]->GetPosition().m128_f32[0] > path.elementAt(0).x + 0.1f)
		{
			enemyArr[2]->enqueueAction(Collision::MoveLeft);
			there = false;
		}

		if (enemyArr[2]->GetPosition().m128_f32[2] < path.elementAt(0).z - 0.1f)
		{
			enemyArr[2]->enqueueAction(Collision::MoveUp);
			there = false;
		}
		else if (enemyArr[2]->GetPosition().m128_f32[2] > path.elementAt(0).z + 0.1f)
		{
			enemyArr[2]->enqueueAction(Collision::MoveDown);
			there = false;
		}

		if (there)
		{
			//path.insertLast(path.elementAt(0));
			path.removeFirst();
		}
	}
	else
	{
		PFTest();
	}

	player->Update(deltaTime);
	player->SetAttackDirection(cursor);
	
	// Update game objects.
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

Collision::Action GameDummy::GetPlayerAction()
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
