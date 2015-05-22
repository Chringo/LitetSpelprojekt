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

	player = new Ent::Player(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
	player->SetMovementSpeed(4.f);

	enemyArrSize = 3;
	enemyMatrixArr = new XMMATRIX[enemyArrSize];
	enemyArr = new Ent::Enemy*[enemyArrSize];
	for (int i = 0; i < enemyArrSize; i++)
	{
		enemyArr[i] = new Ent::Enemy(map->getBaseTiles()[0][i + 3].worldpos);
		enemyArr[i]->SetMovementSpeed(4.f);
		enemyMatrixArr[i] = XMMatrixIdentity();
	}

	setPathfinding(2);

	return S_OK;
}

int GameDummy::floatToIntSpace(float floatCoord)
{
	int counter = 0;
	while (floatCoord - map->TILESIZE > -map->TILESIZE)
	{
		counter++;
		floatCoord -= map->TILESIZE;
	}

	return counter;
}

void GameDummy::setPathfinding(int enemyIndex)
{
	// Allocates 2D bool array used for marking tiles as blocked
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

	// Converting the Enemy float space to int/Tile space and setting as start for A*
	int xs = floatToIntSpace(enemyArr[enemyIndex]->GetPosition().m128_f32[0]);
	int zs = floatToIntSpace(enemyArr[enemyIndex]->GetPosition().m128_f32[2]);
	PF::Pathfinding::Coordinate start = PF::Pathfinding::Coordinate(xs, zs);

	// Converting the Player float space to int/Tile space and setting as start for A*
	int xg = floatToIntSpace(player->GetPosition().m128_f32[0]);
	int zg = floatToIntSpace(player->GetPosition().m128_f32[2]);
	PF::Pathfinding::Coordinate goal = PF::Pathfinding::Coordinate(xg, zg);

	// Makes handling of A* easier. Deallocates the 2D bool array
	PF::Map pfMap = PF::Map(disable, mAmount);

	// Feeding A* with data to deliver a path to target
	LinkedList<PF::Pathfinding::Coordinate> aPath = PF::Pathfinding::Astar(start, goal, pfMap);

	// Removes the last Coordinate so the Enemy won´t try to occypy the same coord as the player
	aPath.removeLast();

	// Converts int/Tile coordinates to float Coordinates
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
			enemyArr[2]->enqueueAction(Ent::MoveRight);
			there = false;
		}
		else if (enemyArr[2]->GetPosition().m128_f32[0] > path.elementAt(0).x + 0.1f)
		{
			enemyArr[2]->enqueueAction(Ent::MoveLeft);
			there = false;
		}

		if (enemyArr[2]->GetPosition().m128_f32[2] < path.elementAt(0).z - 0.1f)
		{
			enemyArr[2]->enqueueAction(Ent::MoveUp);
			there = false;
		}
		else if (enemyArr[2]->GetPosition().m128_f32[2] > path.elementAt(0).z + 0.1f)
		{
			enemyArr[2]->enqueueAction(Ent::MoveDown);
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
		setPathfinding(2);
	}
	//

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
