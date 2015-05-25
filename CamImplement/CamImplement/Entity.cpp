//
//
//

#include "Entity.h"

using namespace DirectX;
using namespace std;
using namespace Collision;

// Entity

Entity::Entity(){}

Entity::~Entity(){}

HRESULT Entity::Update(float deltaTime)
{
	// Set proper movement speed => 'm_Speed' UnitLengths per second.
	m_Move = XMVector3Normalize(m_Move) * deltaTime * m_Speed;

	// Update position.
	m_Position += m_Move;

	return S_OK;
}

void Entity::SetMovementSpeed(float speed)
{
	m_Speed = speed;
}

DirectX::XMMATRIX Entity::GetTransform()
{
	return XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYawFromVector(m_Rotation))
		* XMMatrixTranslationFromVector(m_Position);
}

DirectX::XMVECTOR Entity::GetPosition()
{
	return m_Position;
}

DirectX::XMVECTOR Entity::GetAttackPosition()
{
	XMVECTOR atkPos = XMVectorSet(cosf(XMVectorGetY(m_Rotation)), 0.f, -sinf(XMVectorGetY(m_Rotation)), 0.f);
	atkPos *= m_AttackRange;
	atkPos += m_Position;
	return atkPos;
}

ContainmentType Entity::Intersect(Entity *Entity)
{
	// Super optimized!
	XMVECTOR distance = Entity->m_Position - m_Position;

	if (XMVector3LengthEst(distance).m128_f32[0] > m_Radius + Entity->m_Radius)
		return DISJOINT;

	CollisionHit(Entity);
	
	return INTERSECTS;
}

void Entity::CollisionHit(Entity *entity)
{
	XMVECTOR direction = entity->m_Position - m_Position;
	XMVECTOR force = XMVector3Normalize(direction);
	float length = XMVector3LengthEst(m_Move - entity->m_Move).m128_f32[0];

	// Apply collision force to objects.

}

void Entity::Push(DirectX::XMVECTOR force)
{
	m_Move = force;
}

void Entity::PerformAction(Action action)
{
	// Only handle exlusive actions. Movement works independently.
	if (m_CurrentAction < 4)
		return;

	m_CurrentAction = (action < 4 ? action : Idle);

	// Higher number action overwrite lower.
	switch (action)
	{
	case Attack1:
		// Move sword.
		break;
	case Attack2:
		// Move sword.
		break;
	case Block:
		// Move shield.
		break;
	case Dodge:
		// Tumble away.
		break;

		// Issue moves.
	case MoveUp:
		m_Move.m128_f32[2] = 1.0f;
		break;
	case MoveDown:
		m_Move.m128_f32[2] = -1.0f;
		break;
	case MoveRight:
		m_Move.m128_f32[0] = 1.0f;
		break;
	case MoveLeft:
		m_Move.m128_f32[0] = -1.0f;
		break;
	}
}

Action Entity::GetCurrentAction()
{
	return m_CurrentAction;
}

int Entity::GetCurrentActionFrame()
{
	return m_CurrentActionFrame;
}

// Player

Player::Player(XMVECTOR position, XMVECTOR rotation)
{
	Entity::m_Position = position;
	Entity::m_Rotation = rotation;

	m_Controls[Attack1] = VK_LBUTTON;
	m_Controls[Attack2] = VK_RBUTTON;
	m_Controls[Block] = VK_CONTROL;
	m_Controls[Dodge] = VK_SPACE;
	m_Controls[MoveUp] = 'W';
	m_Controls[MoveDown] = 'S';
	m_Controls[MoveRight] = 'D';
	m_Controls[MoveLeft] = 'A';
}

Player::~Player(){}

HRESULT Player::Update(float deltaTime)
{
	m_Move = XMVectorZero();

	// Key input.
	for (UINT i = 0; i < 8; i++)
		if (KEYDOWN(m_Controls[i]))
			PerformAction((Action)i);

	if (m_CurrentAction != Idle)
	{
		m_CurrentActionFrame++;

		if ((m_CurrentAction == Attack1 || m_CurrentAction == Attack2) && m_CurrentActionFrame == 30)
		{
			m_HitPoints > 0 ? m_HitPoints -= 10.f : m_HitPoints = 100.f;
		}
		if (m_CurrentActionFrame == 60)
		{
			m_CurrentAction = Idle;
			m_CurrentActionFrame = 0;
		}
	}

	// Rotate input.
	if (!XMVector3Equal(m_Move, XMVectorZero()))
		m_Move = XMVector3Rotate(m_Move, XMQuaternionRotationRollPitchYaw(0.f, XM_PIDIV4, 0.f));

	// Update position.
	Entity::Update(deltaTime);

	return S_OK;
}

void Player::SetAttackDirection(POINT clientCursorNDC)
{
	float a = (float)clientCursorNDC.x;
	float b = (float)clientCursorNDC.y;

	// Arbitrary rotation to match mesh
	static float f = 2.4f;	

	m_Rotation.m128_f32[1] = -atan2(a, b) + f;
}

void Player::SetInputKey(Action action, int key)
{
	// Allow multiple non-exclusive bind.
	m_Controls[(int)action] = key;
}

float Player::GetHitPoints()
{
	return m_HitPoints;
}

void Player::Attack()
{
	m_HitPoints > 0 ? m_HitPoints -= 10.f : m_HitPoints = 100.f;
}

// Enemy

Enemy::Enemy(float x, float z)
{
	Entity::m_Position = XMVectorSet(x, 0.f, z, 1.f);
	Entity::m_Rotation = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	orders = LQueue<Action>();
}

Enemy::Enemy(XMFLOAT3 position)
{
	Entity::m_Position = XMVectorSet(position.x, 0.f, position.z, 1.f);
	Entity::m_Rotation = XMVectorSet(0.f, 0.f, 0.f, 1.f);
}

Enemy::~Enemy(){}

HRESULT Enemy::Update(float deltaTime)
{
	m_Move = XMVectorZero();

	while (orders.Size() != 0)
	{
		PerformAction(dequeueAction());
	}

	// Apply movement vector.
	Entity::Update(deltaTime);

	if (m_HitPoints <= 0)
		m_Position = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	return S_OK;
}

void Enemy::enqueueAction(Action action)
{
	orders.Enqueue(action);
}
Action Enemy::dequeueAction()
{
	Action action;
	if (orders.Size() == 0)
	{
		action = Idle;
	}
	else
	{
		action = orders.Dequeue();
	}
	return action;
}
void Enemy::Attack()
{
	m_HitPoints = 0.f;
}