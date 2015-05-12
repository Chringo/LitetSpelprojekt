//
//
//

#ifndef ENTITY__H
#define ENTITY__H

#include <Windows.h>
#include <DirectXCollision.h>
#include <DirectXMath.h>

#define KEYDOWN(vkey)	(GetAsyncKeyState(vkey) & 0x8000)

namespace Collision
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
		Entity();
		virtual ~Entity();

		virtual HRESULT Update(float deltaTime) = 0;

		void SetMovementSpeed(float speed);

		DirectX::XMMATRIX GetTransform();
		DirectX::XMVECTOR GetPosition();

		DirectX::ContainmentType Intersect(Entity *Entity);
		void CollisionHit(Entity *Entity);
		void Push(DirectX::XMVECTOR force);

	protected:
		DirectX::XMVECTOR m_Position;
		DirectX::XMVECTOR m_Rotation;
		const DirectX::XMVECTOR m_Scaling = DirectX::XMVectorSet(1.f, 1.f, 1.f, 1.f);

		float m_Speed = 1.f;
		Action m_CurrentAction = Idle;
		DirectX::XMVECTOR m_Move = DirectX::XMVectorZero();

		void PerformAction(Action action);

	private:
		float m_Mass = 1.f;
		float m_Radius = 1.f;

	};

	// [Move to individual files.]

	class Player : public Entity
	{
	public:
		Player(DirectX::XMVECTOR m_Position,
			DirectX::XMVECTOR m_Rotation);

		virtual ~Player();

		HRESULT Update(float deltaTime);

		void SetAttackDirection(POINT clientCursorNDC);

		void SetInputKey(Action action, int key);

		Action GetCurrentAction();

	private:
		int m_Controls[8];

		// [todo. combat system calculations ]
		unsigned int m_HitPoints = 0;
		unsigned int m_Stamina = 0;
		// Entity *m_Sword;

	};

	class Enemy : public Entity
	{
	public:
		Enemy();
		Enemy(int x, int z);

		virtual ~Enemy();

		void setAction(Action action);
		Action popAction();

		HRESULT Update(float deltaTime);

	private:
		int x, z;
	};

} /// namespace Collision


#endif /// ENTITY__H