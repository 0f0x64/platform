// global defines

#define EditMode false
#define DebugMode false

#define FRAMES_PER_SECOND 60
#define FRAME_LEN 1000 / FRAMES_PER_SECOND

// windows environment

#define _CRT_SECURE_NO_WARNINGS
#define WINDOWS_IGNORE_PACKING_MISMATCH

#include "windows.h"

HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND hWnd;

// gfx & sound

#include <math.h>
#include "timer.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include "DirectXMath.h"
#include <DirectXPackedVector.h>
#include "dx.h"

#include <Xaudio2.h>

// --------------

void Loop()
{
	dx::Clear();
	dx::Present();
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetProcessDPIAware();
	hInst = (HINSTANCE)GetModuleHandle(0);

	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, WndProc, 0,0, hInst, NULL, LoadCursor(NULL, IDC_ARROW),brush, NULL, "fx", NULL };
	RegisterClassEx(&wcex);
	hWnd = CreateWindow("fx", "fx", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInst, NULL);
	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & (~WS_CAPTION));
	
	ShowWindow(hWnd, SW_MAXIMIZE);
	UpdateWindow(hWnd);
	SetFocus(hWnd);
	ShowCursor(EditMode);

	timer::StartCounter();

	dx::InitDevice();

	MSG msg = { 0 };

	while (WM_QUIT != msg.message && !GetAsyncKeyState(VK_ESCAPE))
	{

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)&& msg.message != WM_QUIT)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);				
		}
	
		timer::currentTime = timer::GetCounter();
		timer::fp_delta = timer::currentTime - timer::prevtick;

		Loop();

		auto frameTime = timer::GetCounter() - timer::currentTime;
		auto freeTime = FRAME_LEN - frameTime;

		if (freeTime > .1)
		{
			Sleep(min((int)freeTime, (int)FRAME_LEN));
		}
	}

    return (int) msg.wParam;
}

#if EditMode

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

return 0;
}

#else

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

#endif
