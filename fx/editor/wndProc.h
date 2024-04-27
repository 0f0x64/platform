LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
		break;

	case WM_SETCURSOR:

		switch (editor::ui::cursorID)
		{
		case 0:
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			break;
		case 1:
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			break;
		case 2:
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			break;
		case 3:
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
			break;
		case 4:
			SetCursor(LoadCursor(NULL, IDC_CROSS));
			break;
		case 5:
			SetCursor(LoadCursor(NULL, IDC_IBEAM));
			break;
		}
		break;

	case WM_MOUSEWHEEL:
	{
		auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
		editor::TimeLine::Wheel(delta);
		editor::ViewCam::Wheel(delta);
		break;
	}
	case WM_CHAR:
		break;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
			case VK_SPACE:
				editor::TimeLine::Space();
				break;
			case VK_ESCAPE:
				editor::ViewCam::ToggleViewMode();
				break;

		}
	}
	case WM_LBUTTONUP:
		break;

	case WM_LBUTTONDBLCLK:
		editor::ui::dblClk = true;
	case WM_LBUTTONDOWN:
	{
		editor::ui::mouseLastPos = editor::ui::GetCusorPos();
		editor::TimeLine::lbDown();
		editor::ViewCam::lbDown();
		break;
	}

	case WM_RBUTTONUP:
		break;

	case WM_RBUTTONDOWN:
	{
		editor::ui::mouseLastPos = editor::ui::GetCusorPos();
		editor::TimeLine::rbDown();
		editor::ViewCam::rbDown();
		break;
	}

	case WM_MBUTTONDOWN:
	{
		editor::ui::mouseLastPos = editor::ui::GetCusorPos();
		editor::ViewCam::mbDown();
		break;
	}


	case WM_SYSKEYDOWN:
	{
		if (editor::isKeyDown(VK_LMENU))
		{
			if (editor::isKeyDown(VK_LEFT)) {
				editor::TimeLine::Left();
			}
			if (editor::isKeyDown(VK_RIGHT)) {
				editor::TimeLine::Right();
			}
		}
		break;
	}

	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	default:
		editor::ui::dblClk = false;
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}