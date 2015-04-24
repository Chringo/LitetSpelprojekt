#ifndef INPUT_H
#define INPUT_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <dinput.h>

#define DIRECTION_A DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f)
#define DIRECTION_D DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)
#define DIRECTION_W DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)
#define DIRECTION_S DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f)

class Input
{
private:
	IDirectInputDevice8* m_DIKeyboard;
	IDirectInputDevice8* m_DIMouse;

	LPDIRECTINPUT8 m_directInput;

	BYTE m_keyboardState[256];
	DIMOUSESTATE m_mouseState;
	POINT m_cursorPosition;

	float m_midPointX;
	float m_midPointY;

public:
	Input();
	Input(const Input& obj);
	~Input();

	HRESULT Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport);

	void Update();
	DirectX::XMVECTOR getMoveDirection();
	DirectX::XMVECTOR getMouseDirection();

	void ReleaseCOM();
};

#endif