// global defines

#define EditMode true
#define DebugMode true
#define DirectXDebugMode false

#define FRAMES_PER_SECOND 60
#define FRAME_LEN 1000. / (float) FRAMES_PER_SECOND

float DEMO_DURATION = 5; //in seconds

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

#if EditMode
	#include "editor.h"
#endif

// --------------

void Loop()
{
	
//	double t = timer::frameBeginTime * .01;
	dx::Clear(XMVECTORF32{ .3f,.3f,.3f, 1.f });

	dx::SetRT();

	dx::SetZBuffer();
	dx::NullDrawer(0, 1, 1);

	dx::Present();

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetProcessDPIAware();
	hInst = (HINSTANCE)GetModuleHandle(0);

	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, WndProc, 0,0, hInst, NULL, LoadCursor(NULL, IDC_ARROW),brush, NULL, "fx", NULL };
	RegisterClassEx(&wcex);
	//hWnd = CreateWindow("fx", "fx", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInst, NULL);
	hWnd = CreateWindow("fx", "fx", WS_OVERLAPPEDWINDOW , CW_USEDEFAULT, 0, dx::width, dx::height, NULL, NULL, hInst, NULL);

	dx::init::Init();

	//SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & (~WS_CAPTION));
	ShowWindow(hWnd, EditMode ? SW_SHOW: SW_MAXIMIZE);
	UpdateWindow(hWnd);
	SetFocus(hWnd);
	ShowCursor(EditMode);

	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	#if EditMode
		editor::Init();
	#endif	
	
	dx::CompileShaderFromFile(&dx::Shader[0], L"../fx/projectFiles/shader.hlsl", L"../fx/projectFiles/shader.hlsl");

	MSG msg = { 0 };

	timer::StartCounter();

	while (true)
	{
		double time = timer::GetCounter();
		
		#if EditMode
			
			editor::WatchFiles();

			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		#else
			PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
			if (GetAsyncKeyState(VK_ESCAPE)) break;
			if (timer::currentFrameTime / 1000 > DEMO_DURATION) break;
		#endif

		if (msg.message == WM_QUIT)	break;

		if (time >= timer::nextFrameTime)
		{
			timer::frameBeginTime = timer::GetCounter();

			Loop();

			timer::frameEndTime = timer::GetCounter();
			timer::frameRenderingDuration = timer::frameEndTime - timer::frameBeginTime;
			timer::nextFrameTime = timer::frameBeginTime + FRAME_LEN;
		}

		Sleep((DWORD)min(FRAME_LEN, max(FRAME_LEN - timer::frameRenderingDuration,0)));
		
	}

	ExitProcess(0);

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
