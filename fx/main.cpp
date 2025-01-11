#ifdef REFLECTOR

#include "reflector.h"

#else

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

#if REFLECTION
	#define CALLER_INFO const char* srcFileName, int srcLine 
	#define API(fname, ...) void fname##_impl(__VA_ARGS__)
#else
#define API(fname, ...) void fname##_(__VA_ARGS__)
	#define CALLER_INFO
#endif

#include "generated\apiReflection.h"

#include "projectFiles\loop.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	#if EditMode
		SetProcessDPIAware();
	#endif

	hInst = (HINSTANCE)GetModuleHandle(0);
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, WndProc, 0,0, hInst, NULL, LoadCursor(NULL, IDC_ARROW), brush, NULL, "fx", NULL };
	RegisterClassEx(&wcex);
	hWnd = CreateWindow("fx", "fx", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, hInst, NULL);
	ShowCursor(EditMode);

	#if !EditMode
		ShowWindow(hWnd, SW_MAXIMIZE);
	#endif


	#if EditMode
		editor::Init();
	#endif	

	dx11::Init();

	#if EditMode
		editor::ui::Init();
	#endif	

	MSG msg = { 0 };

	timer::StartCounter();

	while (msg.message != WM_QUIT)
	{
		double time = timer::GetCounter();
		
		#if EditMode
			
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT) break;
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

			#if EditMode
				RECT rect;
				GetClientRect(hWnd, &rect);
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

	#if EditMode
			editor::paramEdit::Save(currentCmd);
	#endif

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

#endif