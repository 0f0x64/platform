void SetRenderWindowPosition()
{
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

				width = dm.dmPelsWidth;
				height = dm.dmPelsHeight;

				return;
			}

			if (GetSystemMetrics(SM_CMONITORS) < 2 || !SECONDARY_DISPLAY_IF_AVAILABLE) // for one monitor conf., just show small window
			{
				MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
				GetMonitorInfo(MonitorFromWindow(hWnd, 0), &monitorInfo);
				const auto rc = monitorInfo.rcMonitor;

				int w = (rc.right - rc.left) / MAIN_DISPLAY_DENOMINATOR;
				int h = (rc.bottom - rc.top) / MAIN_DISPLAY_DENOMINATOR;
				SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, w, h, SWP_NOMOVE);//window on top

				width = dm.dmPelsWidth;
				height = dm.dmPelsHeight;

				return;
			}


		}
		++i;
	}

}