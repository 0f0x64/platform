#ifdef REFLECTOR
	#include "reflector.h"
#else

// windows environment

#define _CRT_SECURE_NO_WARNINGS
#define WINDOWS_IGNORE_PACKING_MISMATCH

#include <windows.h>

HINSTANCE hInst;
HWND hWnd;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ----------------------

#include "math.h"
#include "settings.h" 
#include "tools.h"
#include "timer.h"
#include "utils.h"

#if EditMode
	#include <iostream>
	#include <string>
	#include <filesystem>
	#include <fstream>
	#include <istream>

	#include <regex>
	#include <vector>
	#include <typeinfo>
	#include <source_location>
#endif

int cmdCounter = 0;//reset it in loop start point

#include "types.h"
#include "dx11\dx.h"
#include "Xaudio2.h"
#include "projectFiles\sound\trackStruct.h"
_track track;

using namespace dx11;



#define regDrawer(name)
#define reflect
#define reflect_close
#define cmd(name, ...) __pragma (pack(push,1)) struct CAT(name,_params) {__VA_ARGS__}; __pragma (pack(pop))\
	void name(CAT(name,_params) in)

#if EditMode
	#include "editor\editor.h"
#endif

#include "projectFiles\loop.h"

void UpdateFrame(double time)
{
	#if EditMode

		if (IsIconic(hWnd))
		{
			return;
		}

		if (resize)
		{
 			dx11::Resize();
			resize = false;
		}

	#endif

	if (time >= timer::nextFrameTime)
	{
		timer::frameBeginTime = timer::GetCounter();

		#if !EditMode
			timer::timeCursor = (int)((timer::frameBeginTime - timer::startTime) * SAMPLING_FREQ / 1000.f);
		#endif

		#if EditMode
			editor::UpdateAspect();
			editor::RecompilationCheck(Loop::isPrecalc);
		#endif

		cmdCounter = 0;

		Loop::mainLoop();

		#if EditMode
			editor::Process();
		#endif	

		Draw::Present();

		timer::frameEndTime = timer::GetCounter();
		timer::frameRenderingDuration = timer::frameEndTime - timer::frameBeginTime;
		timer::nextFrameTime = timer::frameBeginTime + FRAME_LEN;
	}

	Sleep((DWORD)min(FRAME_LEN, max(FRAME_LEN - timer::frameRenderingDuration, 0)));

}

//int time_activate = 0;

#if DebugMode
#include <thread>
#include <atomic>

std::atomic<int> g_ExternalScrollDelta{ 0 };
std::atomic<bool> g_Running{ true };

HHOOK hMouseHook;

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {

	if (nCode == HC_ACTION && wParam == WM_MOUSEWHEEL) {
		// Приводим lParam к структуре данных мыши
		MSLLHOOKSTRUCT* pMouse = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

		// Извлекаем значение прокрутки (Wheel Delta)
		// Оно упаковано в HIWORD(mouseData). Делим на 120 (WHEEL_DELTA), чтобы получить кол-во "шагов"
		short zDelta = HIWORD(pMouse->mouseData);

		if (GetKeyState(VK_CONTROL) & 0x8000 || GetKeyState(VK_SHIFT) & 0x8000) {

			int mul = 1;
			if (GetKeyState(VK_SHIFT) & 0x8000) mul *= 10;

			editor::VsTextCurorPos.mouseDelta = zDelta/120*mul;

			HWND activeWnd = GetForegroundWindow();
			DWORD processId;
			GetWindowThreadProcessId(activeWnd, &processId);

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
			if (hProcess) {
				char processName[MAX_PATH];
				DWORD size = MAX_PATH;
				if (QueryFullProcessImageNameA(hProcess, 0, processName, &size)) {
					if (std::string(processName).find("devenv.exe") != std::string::npos) {

						// Здесь можно использовать zDelta для своей задачи
						// Если zDelta > 0 — прокрутка вверх, если < 0 — вниз
						//std::cout << "Intercepted VS Scroll! Delta: " << zDelta << std::endl;

						return 1; // Блокируем стандартный скролл в VS
					}
				}
				CloseHandle(hProcess);
			}
		}
	}
	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}
#endif


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	hInst = (HINSTANCE)GetModuleHandle(0);
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
#if EditMode
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, WndProc, 0,0, hInst, NULL, LoadCursor(NULL, IDC_ARROW), brush, NULL, "fx", NULL };
	RegisterClassEx(&wcex);
	hWnd = CreateWindow(wcex.lpszClassName, wcex.lpszClassName, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, hInst, NULL);
#else
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0,0, hInst, NULL, LoadCursor(NULL, IDC_ARROW), brush, NULL, "fx", NULL };
	RegisterClassEx(&wcex);
	hWnd = CreateWindow(wcex.lpszClassName, wcex.lpszClassName, WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, NULL, NULL, hInst, NULL);
#endif
	
	ShowCursor(EditMode);
	
	dx11::Init();
	

	#if EditMode
		editor::SetRenderWindowPosition();
		editor::Init();

		hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);

		if (!hMouseHook) {
			std::cerr << "Failed to install hook!" << std::endl;
			return 1;
		}
	#else
		ShowWindow(hWnd, SW_MAXIMIZE);
	#endif	

	MSG msg = { 0 };

	UpdateWindow(hWnd);

	timer::StartCounter();

	while (msg.message != WM_QUIT)
	{
		#if EditMode
			
			cmdParamDescBack = cmdParamDesc[currentCmd];

			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					break;
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
				
			}

			editor::WatchFiles();
			editor::WatchForRecompilation();


		#else

			// Проверяем сообщения Windows
			if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}

			//PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
			if (GetAsyncKeyState(VK_ESCAPE) || timer::frameBeginTime / 1000. > DEMO_DURATION)
			{
				break;
			}

		#endif
			
		UpdateFrame(timer::GetCounter());



	}

	#if EditMode
		UnhookWindowsHookEx(hMouseHook);
		editor::SaveAndExit();

	#endif

	ExitProcess(0);
}

#if EditMode

#include "editor\wndProc.h"

#else

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) DestroyWindow(hWnd); // Выход по Esc
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

#endif

#endif