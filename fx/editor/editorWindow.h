#pragma comment(lib, "Dwmapi.lib")
#include <dwmapi.h>


static bool isWin11()
{
	auto sharedUserData = (BYTE*)0x7FFE0000;

	LONG vmaj = *(ULONG*)(sharedUserData + 0x26c); // major version offset
	if (vmaj >= 10)
	{
		LONG vmin = *(ULONG*)(sharedUserData + 0x270);  // minor version offset
		LONG vbm = *(ULONG*)(sharedUserData + 0x260); // build number offset

		if (vbm >= 22000)
		{
			return true;
		}
	}

	return false;
}

HWND vsHWND = NULL;
RECT primaryRC;

static BOOL CALLBACK enumWindowCallback(HWND hWnd, LPARAM lparam) 
{
	int length = GetWindowTextLength(hWnd);
	char* buffer = new char[length + 1];
	GetWindowText(hWnd, buffer, length + 1);
	std::string windowTitle(buffer);
	delete[] buffer;

	if (IsWindowVisible(hWnd) && length != 0) 
	{
		if (std::string::npos != windowTitle.find("platform"))
		{
			vsHWND = hWnd;
		};
	}
	return TRUE;
}

void SetRenderWindowPosition()
{
	RECT rect = { 0,0,0,0 };

	if (isWin11())
	{
		COLORREF DARK_COLOR = 0x00202020;
		DwmSetWindowAttribute(hWnd, DWMWINDOWATTRIBUTE::DWMWA_CAPTION_COLOR, &DARK_COLOR, sizeof(DARK_COLOR));
		DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
	}

	//search for first non-primary display and show window on it
	DWORD i = 0;
	DISPLAY_DEVICE dc = { sizeof(dc) };

	while (EnumDisplayDevices(NULL, i, &dc, EDD_GET_DEVICE_INTERFACE_NAME) != 0)
	{
		if ((dc.StateFlags & DISPLAY_DEVICE_ACTIVE) && !(dc.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
		{

			DEVMODE dm;

			EnumDisplaySettings(dc.DeviceName, ENUM_CURRENT_SETTINGS, &dm);

			bool isPrimary = (dm.dmPosition.x == 0 && dm.dmPosition.y == 0) ? true : false;

			if (!isPrimary && SECONDARY_DISPLAY_IF_AVAILABLE)
			{
				SetWindowPos(hWnd, HWND_TOPMOST, dm.dmPosition.x, dm.dmPosition.y, 0, 0, SWP_NOSIZE);

				//some api calls for correct window maximizing
				MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
				GetMonitorInfo(MonitorFromWindow(hWnd, 0), &monitorInfo);
				const auto rc = monitorInfo.rcMonitor;
				SetWindowPos(hWnd, HWND_TOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 0);
				//SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & (~WS_CAPTION));//no header
				ShowWindow(hWnd, SW_MAXIMIZE);

				//width = dm.dmPelsWidth;
				//height = dm.dmPelsHeight;

				return;
			}

			if (GetSystemMetrics(SM_CMONITORS) < 2 || !SECONDARY_DISPLAY_IF_AVAILABLE) // for one monitor conf., just show small window
			{
				MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
				GetMonitorInfo(MonitorFromWindow(hWnd, 0), &monitorInfo);
				primaryRC = monitorInfo.rcWork;
				auto rc = monitorInfo.rcWork;
				rc.right /= 2.;
				SetWindowPos(hWnd, HWND_TOPMOST, -rect.left, 0, rc.right - rc.left+ rect.left*2+6, rc.bottom+rect.left, SWP_SHOWWINDOW);//window on top

				UpdateWindow(hWnd);
				SetFocus(hWnd);

				#if vsWindowManagement
					EnumWindows(enumWindowCallback, NULL);
					RECT vsRC = { 0,0,0,0 };	
					GetWindowRect(vsHWND,&vsRC);
					SetWindowPos(vsHWND, HWND_TOP, rc.right, vsRC.top, rc.right, vsRC.bottom - vsRC.top, SWP_SHOWWINDOW);
					UpdateWindow(vsHWND);
				#endif	

				GetClientRect(hWnd, &rc);
				//width = rc.right - rc.left;
				//height = rc.bottom - rc.top;

				return;
			}
		}
		++i;
	}

}