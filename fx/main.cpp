#include "settings.h"

// windows environment

#define _CRT_SECURE_NO_WARNINGS
#define WINDOWS_IGNORE_PACKING_MISMATCH
#include <windows.h>
HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND hWnd;

//
#define API void
//

#include <math.h>
#include "timer.h"
#include "utils.h"
#include "types.h"



#include "dx11\dx.h"
#include <Xaudio2.h>
#include "vMachine.h"

using namespace dx11;

#include "generated\constBufReflect.h"

#if EditMode
	#include "editor\cmdEditService.h"
	#include "editor\editor.h"
#endif


#include "projectFiles\loop.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetProcessDPIAware();
	hInst = (HINSTANCE)GetModuleHandle(0);

	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);

	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, WndProc, 0,0, hInst, NULL, LoadCursor(NULL, IDC_ARROW), brush, NULL, "fx", NULL };
	RegisterClassEx(&wcex);
	hWnd = CreateWindow("fx", "fx", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 0, 0, 0, 0, NULL, NULL, hInst, NULL);

	ShowWindow(hWnd, EditMode ? SW_SHOW: SW_MAXIMIZE);
	UpdateWindow(hWnd);
	SetFocus(hWnd);
	ShowCursor(EditMode);

	#if EditMode
		editor::Init();
	#endif	

	Init();

	#if EditMode
		editor::ui::Init();
	#endif	

	MSG msg = { 0 };

	timer::StartCounter();

	while (true)
	{
		double time = timer::GetCounter();
		
		#if EditMode
			
			editor::WatchFiles();

			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

		#else

			PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
			if (GetAsyncKeyState(VK_ESCAPE)) break;
			if (timer::frameBeginTime / 1000. > DEMO_DURATION) break;

		#endif

		if (msg.message == WM_QUIT)	break;

		if (time >= timer::nextFrameTime)
		{
			timer::frameBeginTime = timer::GetCounter();

			//todo: use real rect in pixels without window header
			#if EditMode
				RECT rect;
				GetWindowRect(hWnd, &rect);
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;
				dx11::aspect = float(height) / float(width);
				dx11::iaspect = float(width) / float(height);
			#endif

			Loop::mainLoop();

			#if EditMode
				editor::Process();
			#endif	

			Draw::Present();

			timer::frameEndTime = timer::GetCounter();
			timer::frameRenderingDuration = timer::frameEndTime - timer::frameBeginTime;
			timer::nextFrameTime = timer::frameBeginTime + FRAME_LEN;
		}

		Sleep((DWORD)min(FRAME_LEN, max(FRAME_LEN - timer::frameRenderingDuration,0)));
		
	}

	ExitProcess(0);

}

#if EditMode

#include "editor\wndProc.h"

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
