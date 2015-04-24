#include "Input.h"

Input::Input()
{
	m_DIKeyboard = nullptr;
	m_DIMouse = nullptr;
	m_midPointX = 0.0f;
	m_midPointY = 0.0f;
}

Input::Input(const Input& obj)
{

}

Input::~Input()
{
	
}

HRESULT Input::Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport)
{
	HRESULT hr;

	m_midPointX = viewport.TopLeftX + (float)viewport.Width / 2;
	m_midPointY = viewport.TopLeftY + (float)viewport.Height / 2;

	DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);

	hr = m_directInput->CreateDevice(GUID_SysKeyboard, &m_DIKeyboard, NULL);
	if (FAILED(hr)) { return hr; }

	hr = m_directInput->CreateDevice(GUID_SysMouse, &m_DIMouse, NULL);
	if (FAILED(hr)) { return hr; }

	hr = m_DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) { return hr; }

	hr = m_DIKeyboard->SetCooperativeLevel(wndHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) { return hr; }

	hr = m_DIMouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr)) { return hr; }

	hr = m_DIMouse->SetCooperativeLevel(wndHandle, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	if (FAILED(hr)) { return hr; }

	return hr;
}

void Input::Update()
{
	m_DIKeyboard->Acquire();
	m_DIMouse->Acquire();

	m_DIKeyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	m_DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseState);
	GetCursorPos(&m_cursorPosition);
}

DirectX::XMVECTOR Input::getMoveDirection()
{
	DirectX::XMVECTOR result = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	if (m_keyboardState[DIK_A] & 0x80)
	{
		result = DirectX::XMVectorAdd(result, DIRECTION_A);
	}
	if (m_keyboardState[DIK_D] & 0x80)
	{
		result = DirectX::XMVectorAdd(result, DIRECTION_D);
	}
	if (m_keyboardState[DIK_W] & 0x80)
	{
		result = DirectX::XMVectorAdd(result, DIRECTION_W);
	}
	if (m_keyboardState[DIK_S] & 0x80)
	{
		result = DirectX::XMVectorAdd(result, DIRECTION_S);
	}

	return DirectX::XMVector3Normalize(result);
}

DirectX::XMVECTOR Input::getMouseDirection()
{
	DirectX::XMVECTOR result = DirectX::XMVectorSet(
		-((float)m_cursorPosition.x - m_midPointX),
		0.0f,
		-((float)m_cursorPosition.y - m_midPointY),
		0.0f
		);
	
	if (DirectX::XMVector4Equal(result, DirectX::XMVectorZero()))
	{
		result = DirectX::XMVectorSet(1.0f, 0.0f, 1.0f, 0.0f);
	}

	return DirectX::XMVector3Normalize(result);
}

void Input::ReleaseCOM()
{
	m_DIKeyboard->Release();
	m_DIMouse->Release();
}