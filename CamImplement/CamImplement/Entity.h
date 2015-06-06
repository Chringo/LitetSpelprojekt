#ifndef ENTITY__H
#define ENTITY__H

#include <Windows.h>
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include "PathfindingLib.h"
#include "Map.h"

#include "LQueue.h"

#define KEYDOWN(vkey)	(GetAsyncKeyState(vkey) & 0x8000)
#define DEFAULT_COLOR DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f)

namespace Ent
{
	enum Action
	{
		Attack1,
		Attack2,
		Block,
		Dodge,
		MoveUp,
		MoveDown,
		MoveRight,
		MoveLeft,
		Idle
	};

	// Forward declaration.
	class Obstacle;

	class Entity
	{
	public:
		Entity(DirectX::XMVECTOR position, DirectX::XMFLOAT4 color, float moveSpeed, float scale, float mass, float radius);
		virtual ~Entity();

		virtual HRESULT Update(float deltaTime);

		// Positional
		DirectX::XMMATRIX GetTransform();
		DirectX::XMVECTOR GetPosition();
		DirectX::XMVECTOR GetAttackPosition();
		void SetPosition(float x, float y, float z);
		void SetPosition(DirectX::XMFLOAT3 position);
		void SetScale(float uniformScale);
		void SetRotation(float x, float y, float z);

		// Collision
		bool Intersect(Entity *entity);
		bool Intersect(Obstacle *obstacle);
		void Push(DirectX::XMVECTOR force);

		// Combat
		void SetMovementSpeed(float speed);
		void PerformAction(Action action);
		Action GetCurrentAction();
		int GetCurrentActionFrame();
		float GetHitPoints();

		int GetHitFrameCount();
		bool IsDead();
		virtual void Attack(float mod) = 0;

		DirectX::XMFLOAT4 GetColor() const;

		int getXTileSpace(const float TILESIZE, const float TILEAMOUNT);
		int getZTileSpace(const float TILESIZE, const float TILEAMOUNT);

	protected:
		// Movement data.
		float m_Friction = 0.5f;
		float m_Speed = 1.f;
		DirectX::XMVECTOR m_Position = DirectX::XMVectorZero();
		DirectX::XMVECTOR m_Rotation = DirectX::XMVectorZero();
		DirectX::XMVECTOR m_Scale = DirectX::XMVectorSet(1.f, 1.f, 1.f, 1.f);
		DirectX::XMVECTOR m_Move = DirectX::XMVectorZero();
		DirectX::XMVECTOR m_Force = DirectX::XMVectorZero();

		// Combat data.
		float m_AttackRange = 5.f;
		float m_HitPoints = 100.f;
		Action m_CurrentAction = Idle;
		int m_CurrentActionFrame = 0;
		int m_HitFrameCount = 0;
		bool m_Dead = false;

		int floatToIntSpace(float floatCoord, const float TILESIZE, const float TILEAMOUNT);

	private:
		float m_Mass = 1.f;
		float m_Radius = 1.f;
		DirectX::XMFLOAT4 Color;
	};

	class Player : public Entity
	{
	public:
		Player(DirectX::XMVECTOR position, DirectX::XMVECTOR rotation, float scale);
		Player( DirectX::XMVECTOR position, DirectX::XMVECTOR rotation, DirectX::XMFLOAT4 color, float scale );
		virtual ~Player();

		HRESULT Update(float deltaTime) override;

		void Attack(float mod = 1.f) override;

		void SetAttackDirection(POINT clientCursorNDC);
		void SetInputKey(Action action, int key);

	private:
		int m_Controls[8];

		float m_ActionTimer = 0.f;

		float m_ActionWindUp[8];
		float m_AbilityExecution[8];

	};

	class Enemy : public Entity
	{
	public:
		Enemy(float x, float z, DirectX::XMFLOAT4 color, float scale);
		Enemy(DirectX::XMFLOAT3 position, float scale);
		Enemy(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 color, float scale);

		virtual ~Enemy();

		void enqueueAction(Action action);
		Action dequeueAction();

		HRESULT Update(float deltaTime);
		void Attack(float mod = 1.f) override;
		void SetAttackDirection(DirectX::XMVECTOR dir);

		void setPathfinding(Map* map, PF::Map* pfMap, float goalX, float goalZ);
		void updateMoveOrder();

	private:
		LinkedList<DirectX::XMFLOAT3> path;
		LQueue<Action> orders;
	};

	// Collidable square hitbox obstacle.
	class Obstacle
	{
	public:
		Obstacle(float xPosition, float zPosition, float mass, float xExtend, float zExtend);
		~Obstacle();

		DirectX::BoundingOrientedBox GetBoundingBox();
		DirectX::XMMATRIX GetTransform();

		int getXTileSpace(const float TILESIZE);
		int getZTileSpace(const float TILESIZE);

	private:
		DirectX::BoundingOrientedBox m_Bounds;
		DirectX::XMMATRIX m_Matrix;
		int floatToIntSpace(float floatCoord, const float TILESIZE);
	};

}

#endif /// ENTITY__H