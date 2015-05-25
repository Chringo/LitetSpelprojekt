//
//
//

#include "Entity.h"

using namespace DirectX;
using namespace std;
using namespace Ent;

// Entity

Entity::Entity(XMVECTOR position, float moveSpeed, float mass)
{
	m_Position = position;
	m_Speed = moveSpeed;
	m_Mass = mass;
}

Entity::Entity(float xPosition, float zPosition, float moveSpeed, float mass)
	: Entity(XMVectorSet(xPosition, 0.f, zPosition, 1.f), moveSpeed, mass)
{}

Entity::~Entity(){}

HRESULT Entity::Update(float deltaTime)
{
	// Set proper movement speed => 'm_Speed' UnitLengths per second.
	m_Move = XMVector3Normalize(m_Move) * deltaTime * m_Speed;

	// Update position.
	m_Position += m_Move;
	m_Position += m_Force;

	m_Force *= m_Friction;

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
	return m_Position - XMVectorSet(0.f, 0.f, -m_AttackRange, 0.f) * XMQuaternionRotationRollPitchYawFromVector(m_Rotation);
}

ContainmentType Entity::Intersect(Entity *entity)
{
	// Super optimized!
	XMVECTOR distance = entity->m_Position - m_Position;

	if (XMVector3LengthEst(distance).m128_f32[0] > m_Radius + entity->m_Radius)
		return DISJOINT;

	// Calculate movement versus collision angle.
	float angle0 = XMVector3AngleBetweenVectors(distance, this->m_Move).m128_f32[0];
	float angle1 = XMVector3AngleBetweenVectors(distance, entity->m_Move).m128_f32[0];

	// Mass * movespeed * angular modifier.
	XMVECTOR force0 = XMVector3Normalize(-distance) * this->m_Mass * XMVector3LengthEst(m_Move) * angle0;
	XMVECTOR force1 = XMVector3Normalize(distance) * entity->m_Mass * XMVector3LengthEst(entity->m_Move) * angle1;

	this->Push(force0 - force1);
	entity->Push(force1 - force0);

	return INTERSECTS;
}

void Entity::Push(DirectX::XMVECTOR force)
{
	m_Force = force;
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

int Entity::floatToIntSpace(float floatCoord, const float TILESIZE)
{
	int counter = 0;
	while (floatCoord - TILESIZE > -TILESIZE)
	{
		counter++;
		floatCoord -= TILESIZE;
	}
	return counter;
}

int Entity::getXTileSpace(const float TILESIZE)
{
	return floatToIntSpace(GetPosition().m128_f32[0], TILESIZE);
}
int Entity::getZTileSpace(const float TILESIZE)
{
	return floatToIntSpace(GetPosition().m128_f32[2], TILESIZE);
}

// Player

Player::Player(XMVECTOR position, XMVECTOR rotation)
	: Entity(position.m128_f32[0], position.m128_f32[2], 2.f, 1.f)
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
	: Entity(x, z, 1.f, 1.5f)
{
	Entity::m_Position = XMVectorSet(x, 0.f, z, 1.f);
	Entity::m_Rotation = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	orders = LQueue<Action>();
	path = LinkedList<DirectX::XMFLOAT3>();
}

Enemy::Enemy(XMFLOAT3 position)
	: Entity(position.x, position.z, 1.f, 1.5f)
{
	Enemy(position.x, position.z);
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

void Enemy::setPathfinding(Map* map, PF::Map* pfMap, float goalX, float goalZ)
{
	orders = LQueue<Action>();
	path = LinkedList<DirectX::XMFLOAT3>();

	// Converting the Enemy float space to int/Tile space and setting as start for A*
	int xs = getXTileSpace(map->TILESIZE);
	int zs = getZTileSpace(map->TILESIZE);
	PF::Pathfinding::Coordinate start = PF::Pathfinding::Coordinate(xs, zs);

	// Converting the Player float space to int/Tile space and setting as goal for A*
	int xg = floatToIntSpace(goalX, map->TILESIZE);
	int zg = floatToIntSpace(goalZ, map->TILESIZE);
	PF::Pathfinding::Coordinate goal = PF::Pathfinding::Coordinate(xg, zg);

	if (start != goal)
	{
		// Feeding A* with data to deliver a path to target
		LinkedList<PF::Pathfinding::Coordinate> aPath = PF::Pathfinding::Astar(start, goal, *pfMap);

		// Converts int/Tile coordinates to float Coordinates
		for (int i = 0; i < aPath.size(); i++)
		{
			PF::Pathfinding::Coordinate c = aPath.elementAt(i);
			path.insertLast(map->getBaseTiles()[c.x][c.z].worldpos);
		};
	}
}

void Enemy::updateMoveOrder()
{
	if (path.size() != 0)
	{
		bool there = true;
		if (GetPosition().m128_f32[0] < path.elementAt(0).x - 0.1f)
		{
			enqueueAction(Ent::MoveRight);
			there = false;
		}
		else if (GetPosition().m128_f32[0] > path.elementAt(0).x + 0.1f)
		{
			enqueueAction(Ent::MoveLeft);
			there = false;
		}

		if (GetPosition().m128_f32[2] < path.elementAt(0).z - 0.1f)
		{
			enqueueAction(Ent::MoveUp);
			there = false;
		}
		else if (GetPosition().m128_f32[2] > path.elementAt(0).z + 0.1f)
		{
			enqueueAction(Ent::MoveDown);
			there = false;
		}

		if (there)
		{
			path.removeFirst();
		}
	}
}

