#ifndef GAMEDUMMY_H
#define GAMEDUMMY_H

#include <DirectXMath.h>
#include <sstream>

#include "Map.h"
#include "Entity.h"
#include "Input.h"
#include "PathfindingLib.h"

#define SCALE_SMALL		0.8f
#define SCALE_MEDIUM	1.0f
#define SCALE_LARGE		1.2f

#define BLUE DirectX::XMFLOAT4(0.5f, 0.5f, 2.f, 1.f)
#define GREEN DirectX::XMFLOAT4(0.5f, 2.f, 0.5f, 1.f)
#define YELLOW DirectX::XMFLOAT4(2.f, 2.f, 0.5f, 1.f)

enum GameState
{
	gOngoing,
	gWon,
	gLost,
	gNextLevel
};

class GameDummy
{
private:
	int currentLevel;
	LinkedList<Map*> levels;
	Map* map;

	//---------- Enemy types -----
	const Ent::Enemy regular = Ent::Enemy(DirectX::XMFLOAT3(0, 0, 0), SCALE_MEDIUM, 7.f, 100.0f, 0.9f);
	const Ent::Enemy runner = Ent::Enemy(DirectX::XMFLOAT3(0, 0, 0), SCALE_SMALL, 12.f, 80.0f, 0.6f, GREEN);
	const Ent::Enemy elite = Ent::Enemy(DirectX::XMFLOAT3(0, 0, 0), SCALE_MEDIUM, 8.f, 120.0f, 1.0f, YELLOW);
	const Ent::Enemy giant = Ent::Enemy(DirectX::XMFLOAT3(0, 0, 0), SCALE_LARGE, 6.f, 200.0f, 1.3f, BLUE);
	//----------------------------

	unsigned int obsArrSize;
	Ent::Obstacle** obsArr;
	DirectX::XMMATRIX* obsMatrixArr;
	Ent::Obstacle* worldBounds;

	//- Attack data ------
	bool* hitData[2];
	void CheckPlayerAttack();
	void CheckEnemyAttack(int index);
	//--------------------

	Ent::Player* player;
	// Stores the last tile coords of the player necessary for updating path of enemies
	int lastX;
	int lastZ;

	unsigned int enemyArrSize;
	// Stores the last tile coords of the enemies (necessary for updating path of enemies)
	PF::Pathfinding::Coordinate* lastEnemyCoord;
	// The array of enemies
	Ent::Enemy** enemyArr;
	DirectX::XMMATRIX* enemyMatrixArr;

	void spawnEnemies(int amount, int type);

	POINT clientSize;
	HWND windowHandle;
	float elapsedTime = 0.f;
	int frames = 0;

	// Determinates if the player or the enemies won and marks the game for termination 
	GameState gameState;

public:
	GameDummy();
	~GameDummy();

	void InitLevels();
	void NewGame();

	HRESULT Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport);

	void Update(float deltaTime);

	DirectX::XMMATRIX GetPlayerMatrix();
	DirectX::XMVECTOR GetPlayerPosition();
	DirectX::XMFLOAT4 GetPlayerColor() const;
	Ent::Action GetPlayerAction();

	float GetPlayerHitPoints();

	int GetEnemyArrSize();
	DirectX::XMMATRIX* GetEnemyMatrices();
	DirectX::XMVECTOR GetEnemyPosition(int index);
	DirectX::XMFLOAT4 GetEnemyColor(int index) const;
	Ent::Action GetEnemyAction(int index);
	float GetEnemyHitPoints(int index);

	DirectX::XMMATRIX* GetObsMatrices();
	int GetObsArrSize() const;

	DirectX::XMMATRIX GetMapMatrix() const;
	int GetNrOfTiles() const;

	bool IsPlayerHit();
	bool IsEnemyHit(int index);

	GameState GetGameState() const;
	
	int GetPlayerFrame () const;
	int GetEnemyFrame (int index) const;
};

#endif