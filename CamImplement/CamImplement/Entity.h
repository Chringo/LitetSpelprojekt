#ifndef ENTITY__H
#define ENTITY__H

#include <Windows.h>
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include "PathfindingLib.h"
#include "Map.h"

#include "LQueue.h"

#define KEYDOWN(vkey)	(GetAsyncKeyState(vkey) & 0x8000)

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

	class Entity
	{
	public:
		Entity(DirectX::XMVECTOR position, float moveSpeed, float mass);
		Entity(float xPosition, float zPosition, float moveSpeed, float mass);
		virtual ~Entity();

		virtual HRESULT Update(float deltaTime);

		// Positional
		DirectX::XMMATRIX GetTransform();
		DirectX::XMVECTOR GetPosition();
		DirectX::XMVECTOR GetAttackPosition();

		// Collision
		DirectX::ContainmentType Intersect(Entity *entity);
		void Push(DirectX::XMVECTOR force);

		// Combat
		void SetMovementSpeed(float speed);
		void PerformAction(Action action);
		Action GetCurrentAction();
		int GetCurrentActionFrame();
		virtual void Attack() = 0;

	protected:
		
		// Movement data.
		float m_Friction = 0.7f;
		float m_Speed = 1.f;
		DirectX::XMVECTOR m_Position = DirectX::XMVectorZero();
		DirectX::XMVECTOR m_Rotation = DirectX::XMVectorZero();
		DirectX::XMVECTOR m_Move = DirectX::XMVectorZero();
		DirectX::XMVECTOR m_Force = DirectX::XMVectorZero();

		// Combat data.
		float m_AttackRange = 10.f;
		float m_HitPoints = 100.f;
		Action m_CurrentAction = Idle;
		int m_CurrentActionFrame = 0;

	private:
		float m_Mass = 1.f;
		float m_Radius = 1.f;
	};

	class Player : public Entity
	{
	public:
		Player(DirectX::XMVECTOR position,
			DirectX::XMVECTOR rotation);
		virtual ~Player();

		HRESULT Update(float deltaTime) override;

		void Attack() override;

		void SetAttackDirection(POINT clientCursorNDC);
		void SetInputKey(Action action, int key);

		float GetHitPoints();

	private:
		int m_Controls[8];

	};

	class Enemy : public Entity
	{
	public:
		Enemy(float x, float z);
		Enemy(DirectX::XMFLOAT3 position);

		virtual ~Enemy();

		void enqueueAction(Action action);
		Action dequeueAction();

		HRESULT Update(float deltaTime);
		void Attack() override;

		int getXTileSpace(const float TILESIZE);
		int getZTileSpace(const float TILESIZE);

		void setPathfinding(Map* map, PF::Map* pfMap, float goalX, float goalZ);
		void updateMoveOrder();

		//public for the moment
		LinkedList<DirectX::XMFLOAT3> path;

	private:
		LQueue<Action> orders;
		int floatToIntSpace(float floatCoord, const float TILESIZE);
		
	};

	// Collidable square hitbox obstacle.
	class Obstacle : public Entity
	{
		Obstacle(float xPosition, float zPosition, float mass, float xExtend, float zExtend) : Entity(xPosition, zPosition, 0, mass) {};
		~Obstacle() {};

	};
	

}


#endif /// ENTITY__H