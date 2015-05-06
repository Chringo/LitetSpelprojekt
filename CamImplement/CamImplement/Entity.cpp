//
//
//

#include "Entity.h"

using namespace DirectX;
using namespace std;
using namespace Collision;

// Entity

Entity::Entity()
{}

Entity::~Entity()
{}

void Entity::SetMovementSpeed(float speed)
{
	m_Speed = speed;
}

void Entity::SetPosition(float x, float y, float z)
{
	m_Position = XMVectorSet(x, y, z, 1.f);
}

void Entity::SetRotation(float x, float y, float z)
{
	m_Rotation = XMVectorSet(x, y, z, 1.f);
}

void Entity::SetScale(float uniformScale)
{
	m_Scaling = XMVectorSet(uniformScale, uniformScale, uniformScale, 1.f);
}

DirectX::XMMATRIX Entity::GetTransform()
{
	return XMMatrixScalingFromVector(m_Scaling)
		* XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYawFromVector(m_Rotation))
		* XMMatrixTranslationFromVector(m_Position);
}

DirectX::XMVECTOR Entity::GetPosition()
{
	return m_Position;
}

ContainmentType Entity::Intersect(Entity *Entity)
{
	// Super optimized!
	XMVECTOR dist = Entity->m_Position - m_Position;

	if (XMVector3LengthEst(dist).m128_f32[0] > m_Radius + Entity->m_Radius)
		return DISJOINT;

	CollisionHit(Entity);

	return INTERSECTS;
}

void Entity::CollisionHit(Entity *Entity)
{
	XMVECTOR direction = Entity->m_Position - m_Position;
	XMVECTOR force = XMVector3Normalize(direction);

	// Player VS Static Entity.
	m_Position += XMVector3Reflect(m_Move, force);

	// Player VS Movable Entity. [include force magnitude calculations.]
	Entity->Push(force * XMVector3LengthEst(m_Move) * m_Mass);
}

void Entity::Push(DirectX::XMVECTOR force)
{
	// [collision physics. angular momentum.]
	m_Move = force;
}

// Player

Player::Player(XMVECTOR position, XMVECTOR rotation, XMVECTOR scaling)
{
	Entity::m_Position = position;
	Entity::m_Rotation = rotation;
	Entity::m_Scaling = scaling;

	m_Controls[Attack1] = VK_LBUTTON;
	m_Controls[Attack2] = VK_RBUTTON;
	m_Controls[Block] = VK_CONTROL;
	m_Controls[Dodge] = VK_SPACE;
	m_Controls[MoveUp] = 'W';
	m_Controls[MoveDown] = 'S';
	m_Controls[MoveRight] = 'D';
	m_Controls[MoveLeft] = 'A';
}

Player::~Player()
{

}

HRESULT Player::Update(float deltaTime)
{
	m_Move = XMVectorZero();

	// Key input.
	for (UINT i = 0; i < 8; i++)
		if (KEYDOWN(m_Controls[i]))
			PerformAction((Action)i, deltaTime);

	// Rotate to match camera.
	m_Move = XMVector3Rotate(m_Move, XMQuaternionRotationRollPitchYaw(0.f, XM_PIDIV4, 0.f));

	// Set proper movement speed => 'm_Speed' UnitLengths per second.
	m_Move = XMVector3Normalize(m_Move) * deltaTime * m_Speed;

	// Update position.
	m_Position += m_Move;

	return S_OK;
}

void Player::SetAttackDirection(POINT clientCursorNDC)
{
	float a = clientCursorNDC.x;
	float b = clientCursorNDC.y;

	m_Rotation.m128_f32[1] = -atan2(a, b);
}

void Player::SetInputKey(Action action, int key)
{
	// Allow multiple non-exclusive bind.
	m_Controls[(int)action] = key;
}

void Player::PerformAction(Action action, float deltaTime)
{
	// Only handle exlusive actions. Movement works independently.
	m_CurrentAction = action < 4 ? action : Idle;

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
		case MoveUp:
			m_Move.m128_f32[2] += 1.f;
			break;
		case MoveDown:
			m_Move.m128_f32[2] -= 1.f;
			break;
		case MoveRight:
			m_Move.m128_f32[0] += 1.f;
			break;
		case MoveLeft:
			m_Move.m128_f32[0] -= 1.f;
			break;
	}
}

Action Player::GetCurrentAction()
{
	return m_CurrentAction;
}

// Enemy

Enemy::Enemy(XMVECTOR position, XMVECTOR rotation, XMVECTOR scaling)
{
	Entity::m_Position = position;
	Entity::m_Rotation = rotation;
	Entity::m_Scaling = scaling;
}

Enemy::~Enemy()
{

}

HRESULT Enemy::Update(float deltaTime)
{
	m_Position += m_Move;

	// Friction.
	m_Move *= 0.8f;

	return S_OK;
}