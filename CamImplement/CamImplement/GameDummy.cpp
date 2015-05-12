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

	for (int i = 0; i < tempMapSize; i++)
	{
		delete tempMap[i];
	}
	delete[] tempMap;
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

	tempMap = new XMFLOAT3*[tempMapSize];
	path = LinkedList<XMFLOAT3>();

	for (int x = 0; x < tempMapSize; x++)
	{
		tempMap[x] = new XMFLOAT3[tempMapSize];
		for (int z = 0; z < tempMapSize; z++)
		{
			tempMap[x][z] = XMFLOAT3((float)x, 0, (float)z);
			//path.insertLast(tempMap[x][z]);
		}
	}

	path.insertLast(XMFLOAT3(-5.0f, 0, 5.0f));
	path.insertLast(XMFLOAT3(-5.0f, 0, -5.0f));
	//path.insertLast(XMFLOAT3(5.0f, 0, -5.0f));
	//path.insertLast(XMFLOAT3(5.0f, 0, 5.0f));
	
	player = new Collision::Player(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
	player->SetMovementSpeed(4.f);

	enemyArrSize = 5;
	enemyMatrixArr = new XMMATRIX[enemyArrSize];
	enemyArr = new Collision::Enemy*[enemyArrSize];
	for (int i = 0; i < enemyArrSize; i++)
	{
		enemyArr[i] = new Collision::Enemy(0, i * 5 + 3);
		enemyArr[i]->SetMovementSpeed(2.f);
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
	//POINT cursor;
	//GetCursorPos(&cursor);
	//ScreenToClient(windowHandle, &cursor);

	//// Adjust to client center.
	//cursor.x -= (LONG)(clientSize.x * 0.5f - 5);
	//cursor.y -= (LONG)(clientSize.y * 0.5f - 15);

	player->Update(deltaTime);
	//player->SetAttackDirection(cursor);

	// Temporary example that should be removed later
	bool there = true;
	if (enemyArr[2]->GetPosition().m128_f32[0] < path.elementAt(0).x - 0.1f)
	{
		enemyArr[2]->setAction(Collision::MoveRight);
		there = false;
	}
	else if (enemyArr[2]->GetPosition().m128_f32[0] > path.elementAt(0).x + 0.1f)
	{
		enemyArr[2]->setAction(Collision::MoveLeft);
		there = false;
	}

	if (enemyArr[2]->GetPosition().m128_f32[2] < path.elementAt(0).z - 0.1f)
	{
		enemyArr[2]->setAction(Collision::MoveUp);
		there = false;
	}
	else if (enemyArr[2]->GetPosition().m128_f32[2] > path.elementAt(0).z + 0.1f)
	{
		enemyArr[2]->setAction(Collision::MoveDown);
		there = false;
	}

	if (there)
	{
		path.insertLast(path.elementAt(0));
		path.removeFirst();
	}
	
	//

	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		enemyArr[i]->Update(deltaTime);
		player->Intersect(enemyArr[i]);
		for (size_t j = i + 1; j < (size_t)enemyArrSize; j++)
		{
			player->Intersect(enemyArr[j]);
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
