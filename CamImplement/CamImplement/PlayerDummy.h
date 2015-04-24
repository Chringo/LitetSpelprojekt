
#ifndef PLAYERDUMMY_H
#define PLAYERDUMMY_H

#include<DirectXMath.h>

#define SPEED 0.001f

struct PlayerWorld
{
	DirectX::XMVECTOR position;
	DirectX::XMMATRIX direction;
};

class PlayerDummy
{
private:
	PlayerWorld world;

public:
	PlayerDummy();
	~PlayerDummy();

	void Update(DirectX::XMVECTOR move);
	void setDirection(DirectX::XMVECTOR direction);

	PlayerWorld getData() const;
};

#endif