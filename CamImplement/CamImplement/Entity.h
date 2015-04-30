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
	class Entity
	{
	public:
		Entity();
		virtual ~Entity();

		virtual HRESULT Update(float deltaTime) = 0;

		void SetMovementSpeed(float speed);
		void SetPosition(float x, float y, float z);
		void SetRotation(float x, float y, float z);
		void SetScale(float uniformScale);

		DirectX::XMMATRIX GetTransform();
		DirectX::XMVECTOR GetPosition();

		DirectX::ContainmentType Intersect(Entity *Entity);
		void CollisionHit(Entity *Entity);
		void Push(DirectX::XMVECTOR force);

	protected:
		DirectX::XMVECTOR m_Position;
		DirectX::XMVECTOR m_Rotation;
		DirectX::XMVECTOR m_Scaling;

		float m_Speed = 4.f;
		DirectX::XMVECTOR m_Move = DirectX::XMVectorZero();

	private:
		float m_Mass = 1.f;
		float m_Radius = 1.f;

	};

	// [Move to individual files.]

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

	class Player : public Entity
	{
	public:
		Player(DirectX::XMVECTOR m_Position,
			DirectX::XMVECTOR m_Rotation,
			DirectX::XMVECTOR m_Scaling);

		virtual ~Player();

		HRESULT Update(float deltaTime);

		void SetAttackDirection(POINT clientCursorNDC);

		void PerformAction(Action action,
			float deltaTime);

		void SetInputKey(Action action,
			int key);

		Action GetCurrentAction();

	private:
		int m_Controls[8];
		Action m_CurrentAction = Idle;

		// [todo. combat system calculations ]
		unsigned int m_HitPoints = 0;
		unsigned int m_Stamina = 0;
		// Entity *m_Sword;

	};

	class Enemy : public Entity
	{
	public:
		Enemy(DirectX::XMVECTOR m_Position,
			DirectX::XMVECTOR m_Rotation,
			DirectX::XMVECTOR m_Scaling);

		virtual ~Enemy();

		HRESULT Update(float deltaTime);	// AI-stuff.

	private:

	};

} /// namespace Collision


#endif /// ENTITY__H