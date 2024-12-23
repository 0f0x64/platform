#include "settings.h"
#include "tools.h"

// windows environment

#define _CRT_SECURE_NO_WARNINGS
#define WINDOWS_IGNORE_PACKING_MISMATCH
#include <windows.h>
HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND hWnd;

#if EditMode
	#include <iostream>
	#include <string>
	#include <filesystem>
	#include <fstream>
	#include <regex>
	#include <vector>
	#include <typeinfo>
#endif

int cmdCounter = 0;//reset it in loop start point

#include <math.h>
#include "timer.h"
#include "utils.h"
#include "types.h"

#include "dx11\dx.h"
#include <Xaudio2.h>
#include "vMachine.h"

#include "projectFiles\sound\track_struct.h"

using namespace dx11;

#include "generated\constBufReflect.h"

#if EditMode
	#define regDrawer(name) cmdParamDesc[cmdCounter-1].uiDraw = &name; track_desc.channel[curChannel].cmdIndex = cmdCounter - 1
#define regfuncGroup(name) strcpy(cmdParamDesc[cmdCounter-1].funcGroup, #name); 
	#include "editor\cmdEditService.h"
	#include "editor\editor.h"
	#define REFLECTION true
#else
	#define regDrawer(name)
#endif

#define refPath ./generated/reflection/
#define refTail _ref.h
#define REFLECT(FOO) STRINGIFY( CAT_3(refPath,FOO,refTail) )

#if REFLECTION
	#define REFLECT_CLOSE cmdLevel--
	#define CALLER_INFO const char* srcFileName, int srcLine 
	#define EDITABLE __FILE__, __LINE__
	#define COMMAND(fname, ...) void fname(CALLER_INFO, __VA_ARGS__)
#else
	#define REFLECT_CLOSE 

	#define CALLER_INFO
	#define EDITABLE
	#define COMMAND(fname, ...) void fname(__VA_ARGS__)
#endif

#include "generated\accel.h"
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
			
		if (msg.message == WM_QUIT)	break;

			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			editor::WatchFiles();
			editor::WatchForRecompilation();

		#else

			PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
			if (GetAsyncKeyState(VK_ESCAPE)) break;
			if (timer::frameBeginTime / 1000. > DEMO_DURATION) break;

		#endif

		

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
			#else
				timer::timeCursor = (int)((timer::frameBeginTime - timer::startTime) * SAMPLING_FREQ / 1000.f);
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
