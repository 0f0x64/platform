LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
	{
		if (swapChain)
		{
			RECT r;
			GetWindowRect(hWnd, &r);
			width = r.right - r.left;
			height = r.bottom - r.top;

			context->OMSetRenderTargets(0, 0, 0);

			// Release all outstanding references to the swap chain's buffers.
			if (dx11::Textures::Texture[0].pTexture)
			{
				dx11::Textures::Texture[0].pTexture->Release();
			}

			if (dx11::Textures::Texture[0].pDepth)
			{
				dx11::Textures::Texture[0].pDepth->Release();
			}

			if (dx11::Textures::Texture[0].RenderTargetView[0][0])
			{
				dx11::Textures::Texture[0].RenderTargetView[0][0]->Release();
			}
			if (dx11::Textures::Texture[0].DepthStencilView[0])
			{
				dx11::Textures::Texture[0].DepthStencilView[0]->Release();
			}
			
			HRESULT hr;
			// Preserve the existing buffer count and format.
			// Automatically choose the width and height to match the client rect for HWNDs.
			hr = swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

			// Perform error handling here!

			// Get buffer and create a render-target-view.


			hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
				(void**)&dx11::Textures::Texture[0].pTexture);
			// Perform error handling here!

			//hr = device->CreateRenderTargetView(pBuffer, NULL, &dx11::Textures::Texture[0].RenderTargetView[0][0]);

			//pBuffer->Release();
			
			dx11::Textures::Create(0, dx11::Textures::tType::flat, dx11::Textures::tFormat::u8, XMFLOAT2((float)width, (float)height), false, true);

		}
		break;
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

		editor::paramEdit::Wheel(delta);

		//if (editor::paramEdit::currentParam == -1)
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

		if (editor::editorMode == editor::editorMode_::music)
		{
			editor::paramEdit::trackerKeys(wParam);
		}

		if (editor::editorMode == editor::editorMode_::graphics)
		{

			if ((wParam >= '0' && wParam <= '9') || wParam == VK_OEM_MINUS)
			{
				editor::paramEdit::insertNumber(wParam);
				break;
			}

			switch (wParam)
			{
				case VK_SPACE:
					editor::TimeLine::Space();
					break;
				case VK_ESCAPE:
					editor::ViewCam::ToggleViewMode();
					break;
				case 'A':
					editor::ViewCam::AxisCamYaw(-90);
					break;
				case 'D':
					editor::ViewCam::AxisCamYaw(90);
					break;
				case 'S':
					editor::ViewCam::AxisCamPitch(0);
					break;
				case 'X':
					editor::ViewCam::AxisCamPitch(90);
					break;
				case 'W':
					editor::ViewCam::AxisCamPitch(-90);
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
				case VK_BACK:
					editor::paramEdit::BackSpace();
					break;
				case VK_DELETE:
					editor::paramEdit::Delete();
					break;
			}

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