#include <windows.h>
#include "Graphics.h"

HWND InitWindow(HINSTANCE hInstance, int width, int height);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	const int width = 1280;
	const int height = 720;

	Graphics d3d_Graphics = Graphics();
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance, width, height);

	if (wndHandle)
	{
		if (FAILED(d3d_Graphics.Initialize(wndHandle, hInstance, width, height, 1.0f, 200.0f, false)))
		{
			msg.message = WM_QUIT;
		}

		ShowWindow(wndHandle, nCmdShow);

		// Time keeping.
		INT64 tickPerSecond = 0;
		INT64 previousTime = 0;
		INT64 currentTime = 0;

		QueryPerformanceFrequency((LARGE_INTEGER*)&tickPerSecond);
		QueryPerformanceCounter((LARGE_INTEGER*)&previousTime);
		FLOAT secondsPerTick = 1.0f / (FLOAT)tickPerSecond;

		bool quit = false;
		while (WM_QUIT != msg.message && !quit)
		{
			// Update time.
			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
			FLOAT deltaTime = 0;

			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			// Suspend while application loses focus.
			if (wndHandle == GetFocus())
			{
				deltaTime = (currentTime - previousTime) * secondsPerTick;

				if (!d3d_Graphics.Update(deltaTime))
				{
					quit = true;
				}
				d3d_Graphics.Render();

				d3d_Graphics.SwapFBBuffer();
			}

			// Frame over.
			previousTime = currentTime;
		}

		d3d_Graphics.ReleaseCOM();
		DestroyWindow(wndHandle);
	}

	return (int)msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance, int width, int height)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = L"BasicWindow";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, width, height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	
	HWND handle = CreateWindow(
		L"BasicWindow",
		L"BTH BasicWindow",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return handle;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}