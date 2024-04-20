
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
		break;
	}
	case WM_CHAR:
	case WM_KEYDOWN:
	{
		break;
	}
	case WM_LBUTTONUP:
		editor::ui::lbDown = false;
		break;

	case WM_LBUTTONDBLCLK:
		editor::ui::dblClk = true;
		editor::TimeLine::lbDouble();
	case WM_LBUTTONDOWN:
	{
		editor::ui::mouseLastPos = editor::ui::GetCusorPos();
		editor::ui::lbDown = true;
		editor::TimeLine::lbDown();
		break;
	}

	case WM_RBUTTONUP:
		editor::ui::rbDown = false;
		break;

	case WM_RBUTTONDOWN:
	{
		editor::ui::mouseLastPos = editor::ui::GetCusorPos();
		editor::ui::rbDown = true;
		editor::TimeLine::rbDown();
		break;
	}


	case WM_SYSKEYDOWN:
	{
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