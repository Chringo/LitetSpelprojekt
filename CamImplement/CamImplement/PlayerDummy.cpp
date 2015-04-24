#include "PlayerDummy.h"

PlayerDummy::PlayerDummy()
{
	world.position = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	world.direction = DirectX::XMMatrixIdentity();
}

PlayerDummy::~PlayerDummy()
{

}

void PlayerDummy::Update(DirectX::XMVECTOR move)
{
	move = DirectX::XMVector3ClampLength(move, SPEED, SPEED);
	world.position = DirectX::XMVectorAdd(world.position, move);
}

void PlayerDummy::setDirection(DirectX::XMVECTOR direction)
{
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	world.direction = DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), 
												DirectX::XMMatrixLookAtLH(
															world.position, 
															DirectX::XMVectorAdd(world.position, direction), 
															up
															)
												);
}

PlayerWorld PlayerDummy::getData() const
{
	return world;
}