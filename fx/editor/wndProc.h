LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{


	case WM_SIZE:
	{
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		resize = true;
		return 0;
	}

	case WM_MOUSEMOVE:
		
		break;

	/*case WM_SETCURSOR:

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
		*/


	case WM_MOUSEWHEEL:
	{
		auto delta = GET_WHEEL_DELTA_WPARAM(wParam);

		if (uiContext == uiContext_::camera)
		{
			editor::ViewCam::Wheel(delta);
		}

		if (uiContext == uiContext_::timeline)
		{
			editor::TimeLine::Wheel(delta);
		}

		break;
	}

	case WM_KEYUP:
	{
		break;
	}
	case WM_SYSKEYUP:
	{
		break;
	}


	case WM_CHAR:
		break;
	case WM_KEYDOWN:
	{
		auto isCtrlPressed = GetAsyncKeyState(VK_CONTROL);
		auto isAltPressed = GetAsyncKeyState(VK_MENU);

		switch (wParam)
		{
			case VK_SPACE:
				editor::TimeLine::Space();
				break;
			case VK_ESCAPE:
				editor::ViewCam::ToggleViewMode();
				break;
			case 'A':
				if (isCtrlPressed) editor::ViewCam::AxisCamYaw(-90);
				break;
			case 'D':
				if (isCtrlPressed) editor::ViewCam::AxisCamYaw(90);
				break;
			case 'S':
				if (isCtrlPressed) editor::ViewCam::AxisCamPitch(0);
				break;
			case 'X':
				if (isCtrlPressed) editor::ViewCam::AxisCamPitch(90);
				break;
			case 'W':
				if (isCtrlPressed) editor::ViewCam::AxisCamPitch(-90);
 				break;
			case VK_LEFT:
				editor::paramEdit::cursorPos--;
				break;
			case VK_RIGHT:
				editor::paramEdit::cursorPos++;
				break;
			case VK_HOME:
				editor::paramEdit::cursorPos = 0;;
				break;
			case VK_END:
				editor::paramEdit::cursorPos = INT_MAX;
				break;
		}

		
	}
	case WM_LBUTTONUP:
	{
		editor::paramEdit::action = false;
		break;
	}
	case WM_MBUTTONUP:
	{
		editor::paramEdit::action = false;
		break;
	}
	case WM_RBUTTONUP:
	{
		editor::paramEdit::action = false;
		break;
	}

	case WM_LBUTTONDBLCLK:
		editor::ui::dblClk = true;

	case WM_LBUTTONDOWN:
	{
		editor::ui::mouseLastPos = editor::ui::GetCusorPos();
		editor::ViewCam::lbDown();
		break;
	}

	case WM_MBUTTONDOWN:
	{
		editor::ui::mouseLastPos = editor::ui::GetCusorPos();
		editor::ViewCam::mbDown();
		break;
	}

	case WM_RBUTTONDOWN:
	{
		editor::ui::mouseLastPos = editor::ui::GetCusorPos();
		editor::ViewCam::rbDown();
		editor::paramEdit::rbDown();
		break;
	}

	case WM_SYSKEYDOWN:
	{
		if (editor::isKeyDown(CAM_KEY))
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
	//	editor::ui::dblClk = false;
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}