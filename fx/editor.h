
#define SECONDARY_DISPLAY_IF_AVAILABLE true
#define MAIN_DISPLAY_DENOMINATOR 3

#include <libloaderapi.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

namespace editor
{
	char name[MAX_PATH];

	void SetRenderWindowPosition()
	{
		//search for first non-primary display and show window on it
		DWORD i = 0;
		DISPLAY_DEVICE dc = { sizeof(dc) };

		while (EnumDisplayDevices(NULL, i, &dc, EDD_GET_DEVICE_INTERFACE_NAME) != 0 )
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
					SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & (~WS_CAPTION));//no header
					ShowWindow(hWnd, SW_MAXIMIZE);

					dx::width = dm.dmPelsWidth;
					dx::height = dm.dmPelsHeight;

					return;
				}

				if (GetSystemMetrics(SM_CMONITORS) < 2 || !SECONDARY_DISPLAY_IF_AVAILABLE) // for one monitor conf., just show small window
				{
					MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
					GetMonitorInfo(MonitorFromWindow(hWnd, 0), &monitorInfo);
					const auto rc = monitorInfo.rcMonitor;

					int w = (rc.right-rc.left) / MAIN_DISPLAY_DENOMINATOR;
					int h = (rc.bottom - rc.top) / MAIN_DISPLAY_DENOMINATOR;
					SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, w, h, SWP_NOMOVE);//window on top

					dx::width = dm.dmPelsWidth;
					dx::height = dm.dmPelsHeight;

					return;
				}


			}
			++i;
		}

	}

	void SelfLocate()
	{
		GetModuleFileNameA(NULL, (LPSTR)name, MAX_PATH);
		PathRemoveFileSpec((LPSTR)name);
		SetCurrentDirectory((LPSTR)name);
	}

	bool isWatching = false;
	char path[MAX_PATH] = "../fx/projectFiles";
	HANDLE file = NULL;
	OVERLAPPED overlapped;
	BOOL success = false;
	const int changeBufLen = 1024;
	const int nameBufLen = 1024;
	uint8_t change_buf[changeBufLen];

	void WatchFiles()
	{
		if (!isWatching)//init
		{
			dx::Log("watching for changes: ");
			dx::Log(path);
			dx::Log("\n");

			file = CreateFile(path,
				FILE_LIST_DIRECTORY,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
				NULL);

			assert(file != INVALID_HANDLE_VALUE);
			
			overlapped.hEvent = CreateEvent(NULL, FALSE, 0, NULL);
						
			success = ReadDirectoryChangesW(
				file, change_buf, changeBufLen, TRUE,
				FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
				NULL, &overlapped, NULL);

			isWatching = true;
		}

		//--

		DWORD result = WaitForSingleObject(overlapped.hEvent, 0);

		if (result == WAIT_OBJECT_0)
		{
			DWORD bytes_transferred;
			GetOverlappedResult(file, &overlapped, &bytes_transferred, FALSE);

			FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)change_buf;

			for (;;)
			{
				DWORD name_len = event->FileNameLength / sizeof(wchar_t);

				switch (event->Action)
				{
				case FILE_NOTIFY_CHANGE_FILE_NAME:
				case FILE_ACTION_MODIFIED:
				{
					char fileName[nameBufLen];
					DWORD offset = 0;

					do
					{
						memset(fileName, NULL, nameBufLen);
						event = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&change_buf[offset]);

						WideCharToMultiByte(CP_ACP, NULL, event->FileName, event->FileNameLength / sizeof(WCHAR), fileName, sizeof(fileName), NULL, NULL);

						offset += event->NextEntryOffset;
					} while (event->NextEntryOffset != 0);

					char* s = strstr(fileName, ".hlsl~RF");//VS only hack!
					if (s)
					{
						char s2[nameBufLen];
						memset(s2, NULL, nameBufLen);
						ptrdiff_t bytes = ((char*)s) - ((char*)fileName)+5;//.hlsl length = 5
						memcpy(s2, fileName, bytes);
						s2[bytes + 1] = 0;

						dx::Log("Modified: ");
						dx::Log(s2);
						dx::Log("\n");

						char s3[nameBufLen];

						strcpy(s3, path);
						strcat(s3, "/");
						strcat(s3, s2);

						int len = MultiByteToWideChar(CP_ACP, 0, s3, -1,NULL, 0);
						wchar_t shaderPath [nameBufLen];
						MultiByteToWideChar(CP_ACP, 0, s3, -1,shaderPath, len);
						
						dx::CompileVertexShaderFromFile(&dx::VS[0], shaderPath);
						dx::CompilePixelShaderFromFile(&dx::PS[0], shaderPath);

					}

				}
				break;
				}

				break;

			}

			// Queue the next event
			BOOL success = ReadDirectoryChangesW(
				file, change_buf, 1024, TRUE,
				FILE_NOTIFY_CHANGE_LAST_WRITE,
				NULL, &overlapped, NULL);
		}
	}

	void Init()
	{
		SelfLocate();
		//SetRenderWindowPos();
		SetRenderWindowPosition();
	}
}