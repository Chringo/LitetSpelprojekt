#include "GameDummy.h"

GameDummy::GameDummy()
{
	input = nullptr;
	player = nullptr;
}

GameDummy::~GameDummy()
{
	delete input;
	delete player;
}

HRESULT GameDummy::Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport)
{
	input = new Input();
	player = new PlayerDummy();

	return input->Initialize(wndHandle, hInstance, viewport);
}

void GameDummy::Update()
{
	input->Update();
	player->Update(input->getMoveDirection());
	player->setDirection(input->getMouseDirection());
}

PlayerWorld GameDummy::getPlayerData()
{
	return player->getData();
}

void GameDummy::ReleaseCOM()
{
	input->ReleaseCOM();
}