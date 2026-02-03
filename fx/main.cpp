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

int g_SliderValue = 0;
static void UpdateRangeLabels(HWND hDlg);
static void UpdateSliderValuePosition(HWND hDlg);
HWND g_hDlg = NULL;

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

// Global state
// Цвета в стиле Visual Studio
#define VS_BG_LIGHT    RGB(243, 243, 243)   // фон панелей (светлая тема)
#define VS_TEXT_LIGHT  RGB(30, 30, 30)      // основной текст (светлая)
#define VS_BG_DARK     RGB(37, 37, 38)     // фон панелей (тёмная тема, #252526)
#define VS_TEXT_DARK   RGB(212, 212, 212)  // основной текст (тёмная, #D4D4D4)
static HBRUSH s_hVsBgBrushLight = NULL;
static HBRUSH s_hVsBgBrushDark = NULL;
static HFONT s_hVsFont = NULL;

// DPI окна (Windows 10 1607+ или fallback через GetDeviceCaps)
static UINT GetWindowDpi(HWND hWnd) {
	typedef UINT(WINAPI* GetDpiForWindowFn)(HWND);
	static GetDpiForWindowFn pGetDpiForWindow = NULL;
	static BOOL once = FALSE;
	if (!once) {
		HMODULE hUser = GetModuleHandleW(L"user32.dll");
		if (hUser) pGetDpiForWindow = (GetDpiForWindowFn)GetProcAddress(hUser, "GetDpiForWindow");
		once = TRUE;
	}
	if (pGetDpiForWindow && hWnd)
		return pGetDpiForWindow(hWnd);
	HDC hdc = GetDC(hWnd ? hWnd : GetDesktopWindow());
	UINT dpi = (hdc ? GetDeviceCaps(hdc, LOGPIXELSY) : 96);
	if (hdc) ReleaseDC(hWnd ? hWnd : GetDesktopWindow(), hdc);
	return dpi;
}

// Единый шрифт в стиле Visual Studio (Segoe UI), масштаб по DPI
static HFONT CreateVsFontForDlg(HWND hDlg) {
	UINT dpi = GetWindowDpi(hDlg);
	int height = -MulDiv(9, (int)dpi, 72);
	HFONT hFont = CreateFontW(height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
		DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
	return hFont;
}

// Единая разметка диалога (DPI-масштабируемые пиксели)
static void LayoutSliderDialog(HWND hDlg, HFONT hFont) {
	UINT dpiX = GetWindowDpi(hDlg);
	UINT dpiY = dpiX;
	HDC hdc = GetDC(hDlg);
	if (hdc) { dpiX = GetDeviceCaps(hdc, LOGPIXELSX); dpiY = GetDeviceCaps(hdc, LOGPIXELSY); ReleaseDC(hDlg, hdc); }

	int pad = MulDiv(8, (int)dpiX, 96);
	int labelW = MulDiv(56, (int)dpiX, 96);
	int zoneGap = MulDiv(8, (int)dpiY, 96);

	HWND hSlider = GetDlgItem(hDlg, 1001);
	HWND hText = GetDlgItem(hDlg, 1002);
	HWND hMinLabel = GetDlgItem(hDlg, 1003);
	HWND hMaxLabel = GetDlgItem(hDlg, 1004);
	if (!hSlider || !hText) return;

	if (hFont) {
		SendMessage(hText, WM_SETFONT, (WPARAM)hFont, TRUE);
		if (hMinLabel) SendMessage(hMinLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
		if (hMaxLabel) SendMessage(hMaxLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
	}

	RECT textRect = { 0 };
	GetWindowRect(hText, &textRect);
	int textH = textRect.bottom - textRect.top;
	int topZone = textH + zoneGap;

	RECT dlgRect;
	GetClientRect(hDlg, &dlgRect);
	int dlgW = dlgRect.right - dlgRect.left;

	RECT sliderRect;
	GetWindowRect(hSlider, &sliderRect);
	int sliderH = sliderRect.bottom - sliderRect.top;

	SetWindowPos(hSlider, NULL, pad + labelW, topZone, dlgW - pad * 2 - labelW * 2, sliderH, SWP_NOZORDER);
	if (hMinLabel) SetWindowPos(hMinLabel, NULL, pad, topZone, labelW, sliderH, SWP_NOZORDER);
	if (hMaxLabel) SetWindowPos(hMaxLabel, NULL, dlgW - pad - labelW, topZone, labelW, sliderH, SWP_NOZORDER);
}

// Определяет, включена ли тёмная тема Windows (персонализация приложений)
static BOOL IsVsDarkTheme(void) {
	HKEY hKey = NULL;
	if (RegOpenKeyExW(HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
		0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return FALSE;
	DWORD val = 1, size = sizeof(DWORD);
	BOOL dark = (RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&val, &size) == ERROR_SUCCESS && val == 0);
	RegCloseKey(hKey);
	return dark;
}

// Helper to align memory to DWORD (required for dialog templates)
inline void AlignToDword(std::vector<BYTE>& v) {
	while (v.size() % 4 != 0) v.push_back(0);
}

static void UpdateSliderValuePosition(HWND hDlg) {
	HWND hSlider = GetDlgItem(hDlg, 1001);
	HWND hText = GetDlgItem(hDlg, 1002);
	if (!hSlider || !hText) return;

	RECT dlgRect;
	GetClientRect(hDlg, &dlgRect);
	int dlgW = dlgRect.right - dlgRect.left;
	int dlgH = dlgRect.bottom - dlgRect.top;

	RECT thumbRect;
	SendMessage(hSlider, TBM_GETTHUMBRECT, 0, (LPARAM)&thumbRect);

	POINT ptLeft = { thumbRect.left, thumbRect.top };
	POINT ptRight = { thumbRect.right, thumbRect.bottom };
	MapWindowPoints(hSlider, hDlg, &ptLeft, 1);
	MapWindowPoints(hSlider, hDlg, &ptRight, 1);

	int thumbCenterX = (ptLeft.x + ptRight.x) / 2;
	int thumbTop = (ptLeft.y < ptRight.y) ? ptLeft.y : ptRight.y;

	RECT textRect;
	GetWindowRect(hText, &textRect);
	int textW = textRect.right - textRect.left;
	int textH = textRect.bottom - textRect.top;

	int margin = MulDiv(4, (int)GetWindowDpi(hDlg), 96);
	int newX = thumbCenterX - textW / 2;
	int newY = thumbTop - textH - margin;

	// Ограничение по вертикали: не выше верха окна, не ниже низа
	if (newY < 0) newY = 0;
	if (newY + textH > dlgH) newY = dlgH - textH;
	// Ограничение по горизонтали
	if (newX < 0) newX = 0;
	if (newX + textW > dlgW) newX = dlgW - textW;

	SetWindowPos(hText, NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

// Dialog Procedure
static void UpdateRangeLabels(HWND hDlg) {
	HWND hSlider = GetDlgItem(hDlg, 1001);
	if (!hSlider) return;

	int rangeMin = (int)SendMessage(hSlider, TBM_GETRANGEMIN, 0, 0);
	int rangeMax = (int)SendMessage(hSlider, TBM_GETRANGEMAX, 0, 0);
	TCHAR bufMin[24], bufMax[24];
	wsprintf(bufMin, TEXT("%d"), rangeMin);
	wsprintf(bufMax, TEXT("%d"), rangeMax);

	HWND hMinLabel = GetDlgItem(hDlg, 1003);
	HWND hMaxLabel = GetDlgItem(hDlg, 1004);
	if (hMinLabel) SetWindowText(hMinLabel, bufMin);
	if (hMaxLabel) SetWindowText(hMaxLabel, bufMax);

	UINT dpi = GetWindowDpi(hDlg);
	int pad = MulDiv(8, (int)dpi, 96);
	int labelW = MulDiv(56, (int)dpi, 96);
	RECT dlgRect, sliderRect;
	GetClientRect(hDlg, &dlgRect);
	GetWindowRect(hSlider, &sliderRect);
	MapWindowPoints(HWND_DESKTOP, hDlg, (LPPOINT)&sliderRect, 2);
	int dlgW = dlgRect.right - dlgRect.left;
	int topY = sliderRect.top;
	int labelH = sliderRect.bottom - sliderRect.top;
	if (hMinLabel) SetWindowPos(hMinLabel, NULL, pad, topY, labelW, labelH, SWP_NOZORDER);
	if (hMaxLabel) SetWindowPos(hMaxLabel, NULL, dlgW - pad - labelW, topY, labelW, labelH, SWP_NOZORDER);
}

INT_PTR CALLBACK SliderDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {

	if (!IsWindowVisible(hDlg)) {
		// Разрешаем только критические системные сообщения
		if (message != WM_SHOWWINDOW && message != WM_DESTROY && message != WM_INITDIALOG)
			return FALSE;
	}

	switch (message) {
	case WM_DESTROY:
		if (s_hVsFont) { DeleteObject(s_hVsFont); s_hVsFont = NULL; }
		if (s_hVsBgBrushLight) { DeleteObject(s_hVsBgBrushLight); s_hVsBgBrushLight = NULL; }
		if (s_hVsBgBrushDark) { DeleteObject(s_hVsBgBrushDark);  s_hVsBgBrushDark = NULL; }
		return FALSE;
	case WM_CTLCOLORDLG:
		return (INT_PTR)(IsVsDarkTheme() ? s_hVsBgBrushDark : s_hVsBgBrushLight);
	case WM_CTLCOLORSTATIC: {
		HDC hdc = (HDC)wParam;
		BOOL dark = IsVsDarkTheme();
		SetBkColor(hdc, dark ? VS_BG_DARK : VS_BG_LIGHT);
		SetTextColor(hdc, dark ? VS_TEXT_DARK : VS_TEXT_LIGHT);
		return (INT_PTR)(dark ? s_hVsBgBrushDark : s_hVsBgBrushLight);
	}
	case WM_CTLCOLORSCROLLBAR:
		return (INT_PTR)(IsVsDarkTheme() ? s_hVsBgBrushDark : s_hVsBgBrushLight);
	case WM_THEMECHANGED:
		InvalidateRect(hDlg, NULL, TRUE);
		return FALSE;
	case WM_DPICHANGED: {
		if (s_hVsFont) { DeleteObject(s_hVsFont); s_hVsFont = NULL; }
		s_hVsFont = CreateVsFontForDlg(hDlg);
		LayoutSliderDialog(hDlg, s_hVsFont);
		UpdateRangeLabels(hDlg);
		UpdateSliderValuePosition(hDlg);
		return TRUE;
	}
	case WM_INITDIALOG: {
		if (!s_hVsBgBrushLight) s_hVsBgBrushLight = CreateSolidBrush(VS_BG_LIGHT);
		if (!s_hVsBgBrushDark)  s_hVsBgBrushDark = CreateSolidBrush(VS_BG_DARK);
		if (s_hVsFont) { DeleteObject(s_hVsFont); s_hVsFont = NULL; }
		s_hVsFont = CreateVsFontForDlg(hDlg);

		HWND hSlider = GetDlgItem(hDlg, 1001);
		HWND hText = GetDlgItem(hDlg, 1002);
		SendMessage(hSlider, TBM_SETRANGE, TRUE, MAKELPARAM(-100, 100));
		SendMessage(hSlider, TBM_SETPOS, TRUE, g_SliderValue);
		SetDlgItemInt(hDlg, 1002, g_SliderValue, TRUE);

		// Подписи минимума/максимума — создаём при отсутствии (разметка и шрифт в LayoutSliderDialog)
		if (!GetDlgItem(hDlg, 1003)) {
			CreateWindowExW(0, L"Static", L"", WS_CHILD | WS_VISIBLE | SS_LEFT,
				0, 0, 50, 14, hDlg, (HMENU)(UINT_PTR)1003, NULL, NULL);
		}
		if (!GetDlgItem(hDlg, 1004)) {
			CreateWindowExW(0, L"Static", L"", WS_CHILD | WS_VISIBLE | SS_RIGHT,
				0, 0, 50, 14, hDlg, (HMENU)(UINT_PTR)1004, NULL, NULL);
		}

		LayoutSliderDialog(hDlg, s_hVsFont);
		UpdateRangeLabels(hDlg);
		UpdateSliderValuePosition(hDlg);
		return TRUE;
	}
	case WM_HSCROLL: {
		int newVal = (int)SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
		g_SliderValue = newVal;
		SetDlgItemInt(hDlg, 1002, g_SliderValue, TRUE);
		UpdateRangeLabels(hDlg);   // обновить подписи при изменении диапазона
		UpdateSliderValuePosition(hDlg);
		return TRUE;
	}
	case WM_LBUTTONDOWN:
		SendMessage(hDlg, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		return TRUE;
	}
	return FALSE;
}


void CreateSliderDialog(HINSTANCE hInst, HWND hParent) {
	std::vector<BYTE> buffer;

	// 1. Define Dialog Header
	DLGTEMPLATE dlg;
	dlg.style = WS_POPUP | WS_BORDER | DS_SETFONT | DS_CENTER;
	dlg.dwExtendedStyle = 0;
	dlg.cdit = 2; // 2 controls: Slider and Text
	dlg.x = 0; dlg.y = 0; dlg.cx = 200; dlg.cy = 30;

	buffer.resize(sizeof(DLGTEMPLATE));
	memcpy(buffer.data(), &dlg, sizeof(DLGTEMPLATE));

	// Append Menu (0), Class (0), Title (0)
	for (int i = 0; i < 3; ++i) { buffer.push_back(0); buffer.push_back(0); }

	// Шрифт в стиле Visual Studio — Segoe UI
	short fontSize = 9;
	buffer.insert(buffer.end(), (BYTE*)&fontSize, (BYTE*)&fontSize + 2);
	const wchar_t* fontName = L"Segoe UI";
	buffer.insert(buffer.end(), (BYTE*)fontName, (BYTE*)fontName + (wcslen(fontName) + 1) * 2);

	// 2. Add Slider Control (Trackbar)
	AlignToDword(buffer);
	DLGITEMTEMPLATE item1;
	item1.style = WS_CHILD | WS_VISIBLE ;
	item1.dwExtendedStyle = 0;
	item1.x = 5; item1.y = 7; item1.cx = dlg.cx - 40; item1.cy = 15;
	item1.id = 1001;

	size_t start = buffer.size();
	buffer.resize(start + sizeof(DLGITEMTEMPLATE));
	memcpy(&buffer[start], &item1, sizeof(DLGITEMTEMPLATE));

	const wchar_t* sliderClass = L"msctls_trackbar32";
	buffer.insert(buffer.end(), (BYTE*)sliderClass, (BYTE*)sliderClass + (wcslen(sliderClass) + 1) * 2);
	buffer.push_back(0); buffer.push_back(0); // Title
	buffer.push_back(0); buffer.push_back(0); // Creation Data

	// 3. Add Static Text
	AlignToDword(buffer);
	DLGITEMTEMPLATE item2;
	item2.style = WS_CHILD | WS_VISIBLE | SS_LEFT;
	item2.dwExtendedStyle = 0;
	item2.x = dlg.cx - 30; item2.y = 10; item2.cx = 25; item2.cy = 10;
	item2.id = 1002;

	start = buffer.size();
	buffer.resize(start + sizeof(DLGITEMTEMPLATE));
	memcpy(&buffer[start], &item2, sizeof(DLGITEMTEMPLATE));

	const wchar_t* staticClass = L"Static";
	buffer.insert(buffer.end(), (BYTE*)staticClass, (BYTE*)staticClass + (wcslen(staticClass) + 1) * 2);
	buffer.push_back(0); buffer.push_back(0); // Title
	buffer.push_back(0); buffer.push_back(0); // Creation Data

	// Create the non-modal dialog
	g_hDlg = CreateDialogIndirect(hInst, (LPDLGTEMPLATE)buffer.data(), hParent, SliderDlgProc);

	if (g_hDlg) {
		// HWND_TOPMOST ensures it stays above even non-active windows
		SetWindowPos(g_hDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		//	ShowWindow(g_hDlg, SW_SHOW);
	}
	//ShowWindow(g_hDlg, SW_SHOW);
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
		CreateSliderDialog(hInst, hWnd);

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