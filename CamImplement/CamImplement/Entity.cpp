//
//
//

#include "Entity.h"

using namespace DirectX;
using namespace std;
using namespace Ent;

/**********************************************************************************/
/************************************* Entity *************************************/

Entity::Entity(XMVECTOR position, XMFLOAT4 color, float moveSpeed, float scale, float mass, float radius)
{
	m_Position = position;
	Color = color;
	m_Speed = moveSpeed;
	m_Mass = mass;
	m_Radius = radius;

	// Uniform scaling only.
	m_Scale = XMVectorSet(scale, scale, scale, 1.f);
	m_Radius *= scale;
}

Entity::~Entity(){}

HRESULT Entity::Update(float deltaTime)
{
	// Set proper movement speed => 'm_Speed' UnitLengths per second.
	m_Move = XMVector3Normalize(m_Move) * deltaTime * m_Speed;

	// Reduce friction.
	m_Force *= m_Friction;

	// Apply movement and force to position.
	m_Position += m_Move;
	m_Position += m_Force;

	if (m_HitFrameCount > 0)
		m_HitFrameCount < 20 ? m_HitFrameCount++ : m_HitFrameCount = 0;

	// Update animation if we're supposed to.
	if (m_isAnimating == true)
	{
		m_currentFrame++;
		if (m_currentFrame == m_totalFrames)
		{
			m_currentFrame = 0;
		}
	}

	if (!IsDead ())
	{
		if (XMVector3Equal (m_Move, XMVectorZero ()) && (m_CurrentAction == Idle))
		{
			m_isAnimating = false;		//If the entity isn't moving anymore we want
			m_currentFrame = 0;			//to stop the walk animation, regardless of if
		}								//the walk animation actually finished or not.
	}



	return S_OK;
}

void Entity::SetMovementSpeed(float speed)
{
	m_Speed = speed;
}

DirectX::XMMATRIX Entity::GetTransform()
{
	return XMMatrixScalingFromVector(m_Scale)
		* XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYawFromVector(m_Rotation))
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

void Entity::SetPosition(float x, float y, float z)
{
	m_Position = XMVectorSet(x, y, z, 1.f);
}

void Entity::SetPosition(DirectX::XMFLOAT3 position)
{
	m_Position = XMVectorSet(position.x, position.y, position.z, 1.0f);
}

void Entity::SetScale(float uniformScale)
{
	m_Scale = XMVectorSet(uniformScale, uniformScale, uniformScale, 1.f);
}

void Entity::SetRotation(float x, float y, float z)
{
	m_Rotation = XMVectorSet(x, 90.0f, z, 1.f);
}

bool Entity::Intersect(Entity *entity)
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

bool Entity::Intersect(Obstacle *obstacle)
{
	// Create bounding sphere.
	BoundingSphere s;
	XMStoreFloat3(&s.Center, m_Position);
	s.Radius = m_Radius;

	ContainmentType containment = obstacle->GetBoundingBox().Contains(s);
	switch (containment)
	{
		case DISJOINT:
			break;
		case CONTAINS:
			break;
		case INTERSECTS:
			// Undo movement.
			m_Position -= m_Move;
			m_Position -= m_Force;
			break;
	}

	return containment == INTERSECTS;
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
		m_currentFrame = m_animAttack;
		break;
	case Attack2:
		// Move sword.
		break;
	case Block:
		// Move shield.
		m_currentFrame = m_animBlock;
		break;
	case Dodge:
		// Tumble away.
		m_currentFrame = m_animDodge;
		break;

		// Issue moves.
	case MoveUp:
		m_Move.m128_f32[2] = 1.0f;
		if (m_currentFrame < m_animMoveStart || m_currentFrame > m_animMoveEnd)
		{
			m_currentFrame = m_animMoveStart;
			m_isAnimating = true;
		}
		else if (m_currentFrame == 80)
		{
			m_currentFrame = m_animMove;
			m_isAnimating = true;
		}
		break;
	case MoveDown:
		m_Move.m128_f32[2] = -1.0f;
		if (m_currentFrame < m_animMoveStart || m_currentFrame > m_animMoveEnd)
		{
			m_currentFrame = m_animMove;
			m_isAnimating = true;
		}
		else if (m_currentFrame == 80)
		{
			m_currentFrame = m_animMove;
			m_isAnimating = true;
		}
		break;
	case MoveRight:
		m_Move.m128_f32[0] = 1.0f;
		if (m_currentFrame < m_animMoveStart || m_currentFrame > m_animMoveEnd)
		{
			m_currentFrame = m_animMove;
			m_isAnimating = true;
		}
		else if (m_currentFrame == 80)
		{
			m_currentFrame = m_animMove;
			m_isAnimating = true;
		}
		break;
	case MoveLeft:
		m_Move.m128_f32[0] = -1.0f;
		if (m_currentFrame < m_animMoveStart || m_currentFrame > m_animMoveEnd)
		{
			m_currentFrame = m_animMove;
			m_isAnimating = true;
		}
		else if (m_currentFrame == 80)
		{
			m_currentFrame = m_animMove;
			m_isAnimating = true;
		}
		break;
	}
}

Action Entity::GetCurrentAction()
{
	return m_CurrentAction;
}

float Entity::GetAttackValue()
{
	float attackValue = 0.0f;
	if (m_CurrentAction == Ent::Attack1 && m_CurrentActionFrame == 30)
	{
		attackValue = 40.0f;
	}
	else if(m_CurrentAction == Ent::Attack2 && m_CurrentActionFrame == 60)
	{
		attackValue = 60.0f;
	}
	return attackValue * m_AttackStrength;
}

void Entity::DecreaseHealth(float damage)
{
	m_HitPoints -= damage;
	if (m_HitPoints <= 0.f)
	{
		m_currentFrame = 96;
		m_isAnimating = true;
	}
}

int Entity::GetCurrentActionFrame()
{
	return m_CurrentActionFrame;
}

float Entity::GetHitPoints()
{
	return m_HitPoints;
}

int Entity::GetHitFrameCount()
{
	return m_HitFrameCount;
}

bool Entity::IsDead()
{
	return m_Dead;
}

DirectX::XMFLOAT4 Entity::GetColor() const
{
	return Color;
}

int Entity::getXTileSpace(const float TILESIZE, const float TILEAMOUNT)
{
	return floatToIntSpace(GetPosition().m128_f32[0], TILESIZE, TILEAMOUNT);
}
int Entity::getZTileSpace(const float TILESIZE, const float TILEAMOUNT)
{
	return floatToIntSpace(GetPosition().m128_f32[2], TILESIZE, TILEAMOUNT);
}

int Entity::floatToIntSpace(float floatCoord, const float TILESIZE, const float TILEAMOUNT)
{
	int counter = 0;
	while (floatCoord - TILESIZE > -TILESIZE)
	{
		counter++;
		floatCoord -= TILESIZE;
	}
	if (counter > TILEAMOUNT - 1) counter = TILEAMOUNT - 1;
	return counter;
}

int Entity::GetFrame ()
{
	return m_currentFrame;
}

/**********************************************************************************/
/************************************* Player *************************************/

Player::Player(XMVECTOR position, float scale, float moveSpeed)
	: Entity(position, DEFAULT_COLOR, moveSpeed, scale, 1.f, 1.f)
{
	Entity::m_Position = position;
	Entity::m_Rotation = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	m_Controls[Attack1] = VK_LBUTTON;
	//m_Controls[Attack2] = VK_RBUTTON;
	m_Controls[Block] = VK_CONTROL;
	m_Controls[Dodge] = VK_SPACE;
	m_Controls[MoveUp] = 'W';
	m_Controls[MoveDown] = 'S';
	m_Controls[MoveRight] = 'D';
	m_Controls[MoveLeft] = 'A';

	// Set animation frame data (where frames start)
	m_totalFrames = 191;
	m_currentFrame = 0;
	m_animAttack = 2;
	m_animBlock = 99;
	m_animDodge = 127;
	m_animMove = 61;
	m_animMoveStart = 41;
	m_animMoveEnd = 98;
}

Player::Player(XMVECTOR position, XMFLOAT4 color, float scale, float moveSpeed)
	: Entity(position, color, moveSpeed, scale, 1.f, 1.f)
{
	Entity::m_Position = position;
	Entity::m_Rotation = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	m_Controls[Attack1] = VK_LBUTTON;
	m_Controls[Attack2] = VK_RBUTTON;
	m_Controls[Block] = VK_CONTROL;
	m_Controls[Dodge] = VK_SPACE;
	m_Controls[MoveUp] = 'W';
	m_Controls[MoveDown] = 'S';
	m_Controls[MoveRight] = 'D';
	m_Controls[MoveLeft] = 'A';

	// Set animation frame data (where frames start)
	m_totalFrames = 191;
	m_currentFrame = 0;
	m_animAttack = 2;
	m_animBlock = 99;
	m_animDodge = 127;
	m_animMove = 41;
	m_animMoveStart = 41;
	m_animMoveEnd = 98;
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

		int frameLimit = 0;
		switch (m_CurrentAction)
		{
		//Combat moves
		case Attack1:
			frameLimit = 38;
			m_isAnimating = true;
			break;
		case Attack2:
			frameLimit = 70;
			m_isAnimating = true;
			break;
		case Block:
			frameLimit = 27;
			m_isAnimating = true;
			break;
		case Dodge:
			frameLimit = 65;
			m_isAnimating = true;
			break;
		}
			
		if (m_CurrentActionFrame == frameLimit)
		{
			m_CurrentAction = Idle;
			m_CurrentActionFrame = 0;
			m_isAnimating = false;
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

void Player::DecreaseHealth(float damage)
{
	if (m_CurrentAction == Dodge) return;
	if (m_CurrentAction == Block) damage /= 8;

	m_HitPoints -= damage;
	m_Dead = m_HitPoints <= 0.f;
}

/**********************************************************************************/
/************************************* Enemy  *************************************/

Enemy::Enemy(float x, float z, DirectX::XMFLOAT4 color, float scale)
	: Entity(XMVectorSet(x, 0.f, z, 1.f), color, 1.f, scale, 1.f, 1.5f)
{
	Entity::m_Position = XMVectorSet(x, 0.f, z, 1.f);
	Entity::m_Rotation = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	orders = LQueue<Action>();
	path = LinkedList<DirectX::XMFLOAT3>();

	// Set animation frame data (where frames start)
	m_totalFrames = 134;
	m_currentFrame = 0;
	m_animAttack = 56;
	m_animMove = 1;
	m_animMoveStart = 1;
	m_animMoveEnd = 55;
	m_animDeath = 96;
}

Enemy::Enemy(XMFLOAT3 position, float scale, float moveSpeed, float healthPoints, float attackStrength)
	: Entity(XMVectorSet(position.x, position.y, position.z, 1.f), DEFAULT_COLOR, moveSpeed, scale, 1.f, 1.5f)
{
	Enemy(position.x, position.z, DEFAULT_COLOR, scale);
	m_HitPoints = healthPoints;
	m_AttackStrength = attackStrength;

	// Set animation frame data (where frames start)
	m_totalFrames = 134;
	m_currentFrame = 0;
	m_animAttack = 56;
	m_animMove = 1;
	m_animMoveStart = 1;
	m_animMoveEnd = 55;
	m_animDeath = 96;
}

Enemy::Enemy(XMFLOAT3 position, float scale, float moveSpeed, float healthPoints, float attackStrength, DirectX::XMFLOAT4 color)
	: Entity(XMVectorSet(position.x, position.y, position.z, 1.f), color, moveSpeed, scale, 1.f, 1.5f)
{
	Enemy(position.x, position.z, color, scale);
	m_HitPoints = healthPoints;
	m_AttackStrength = attackStrength;

	// Set animation frame data (where frames start)
	m_totalFrames = 134;
	m_currentFrame = 0;
	m_animAttack = 56;
	m_animMove = 1;
	m_animMoveStart = 1;
	m_animMoveEnd = 55;
	m_animDeath = 96;
}

Enemy::~Enemy(){}

HRESULT Enemy::Update(float deltaTime)
{
	if (m_currentFrame < 96)
	{
		m_Move = XMVectorZero ();

		while (orders.Size () != 0)
		{
			PerformAction (dequeueAction ());
		}

		if (m_CurrentAction != Idle)
		{
			m_CurrentActionFrame++;

			int frameLimit = 0;
			switch (m_CurrentAction)
			{
			case Attack1:
				frameLimit = 39;
				break;
			case Attack2:
				frameLimit = 70;
				break;
			case Block:
				frameLimit = 30;
				break;
			case Dodge:
				frameLimit = 40;
				break;
			default:
				break;
			}

			if (m_CurrentActionFrame == frameLimit)
			{
				m_CurrentAction = Idle;
				m_CurrentActionFrame = 0;
			}
		}

		// Rotate to match move vector.
		float a = m_Move.m128_f32[0];
		float b = m_Move.m128_f32[2];
		m_Rotation.m128_f32[1] = -atan2 (a, b);

		// Apply movement vector.
		Entity::Update (deltaTime);
	}
	else
	{
		m_currentFrame++;
	}

	if (m_currentFrame == 134)
	{
		m_Dead = true;
		m_isAnimating = false;
	}

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

void Enemy::SetAttackDirection(DirectX::XMVECTOR dir)
{
	float a = DirectX::XMVectorGetX(dir);
	float b = DirectX::XMVectorGetZ(dir);

	static float f = 2.4f;// Will be removed as soon as meshes have been corrected

	m_Rotation.m128_f32[1] = -atan2(a, b) + f;
}

void Enemy::setPathfinding(Map* map, PF::Map* pfMap, float goalX, float goalZ)
{
	orders = LQueue<Action>();
	path = LinkedList<DirectX::XMFLOAT3>();

	// Converting the Enemy float space to int/Tile space and setting as start for A*
	int xs = getXTileSpace(map->TILESIZE, map->getChunkSize());
	int zs = getZTileSpace(map->TILESIZE, map->getChunkSize());
	PF::Pathfinding::Coordinate start = PF::Pathfinding::Coordinate(xs, zs);

	// Converting the Player float space to int/Tile space and setting as goal for A*
	int xg = floatToIntSpace(goalX, map->TILESIZE, map->getChunkSize());
	int zg = floatToIntSpace(goalZ, map->TILESIZE, map->getChunkSize());
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

/**********************************************************************************/
/************************************ Obstacle ************************************/
Obstacle::Obstacle(float xPosition, float zPosition, float xExtend, float zExtend, float rot)
{
	// Create boundingbox.
	m_Bounds.Center = XMFLOAT3(xPosition, 0.f, zPosition);
	m_Bounds.Extents = XMFLOAT3(xExtend, 10.f, zExtend);

	// Rotation
	float rad = (2 * XM_PI);
	rad *= rot;

	XMVECTOR orientation = XMVectorSet(0.f, rad, 0.f, 0.f);
	//XMStoreFloat4(&m_Bounds.Orientation, orientation);// If collision should be rotated

	// Create world matrix.
	float s = 0.8f;
	m_Matrix = XMMatrixScalingFromVector(XMVectorSet(s, s, s, 1.f))
		* XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYawFromVector(orientation))
		* XMMatrixTranslation(xPosition, 0.f, zPosition);
};

Obstacle::~Obstacle() {};

DirectX::BoundingOrientedBox Obstacle::GetBoundingBox()
{
	return m_Bounds;
}

DirectX::XMMATRIX Obstacle::GetTransform()
{
	return m_Matrix;
}

int Obstacle::getXTileSpace(const float TILESIZE)
{
	return floatToIntSpace(m_Bounds.Center.x, TILESIZE);
}
int Obstacle::getZTileSpace(const float TILESIZE)
{
	return floatToIntSpace(m_Bounds.Center.z, TILESIZE);
}

int Obstacle::floatToIntSpace(float floatCoord, const float TILESIZE)
{
	int counter = 0;
	while (floatCoord - TILESIZE > -TILESIZE)
	{
		counter++;
		floatCoord -= TILESIZE;
	}
	return counter;
}
