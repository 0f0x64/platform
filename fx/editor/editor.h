using namespace dx11;



std::vector<std::string> regex_split(const std::string& str, const std::regex& reg) {
	const std::sregex_token_iterator beg{ str.cbegin(), str.cend(), reg, -1 };
	const std::sregex_token_iterator end{};

	return { beg, end };
}

enum class DragContext : int { free, timeCursor, timeKey, cameraView, cameraButtons, clipMove };
DragContext lbDragContext = DragContext::free;

enum class uiContext_ : int { stack, camera, timeline };
uiContext_ uiContext = uiContext_::stack;

namespace editor
{
	#include <libloaderapi.h>
	#include <shlwapi.h>
	#pragma comment(lib, "Shlwapi.lib")

	char name[MAX_PATH];
	void SelfLocate()
	{
		GetModuleFileNameA(NULL, (LPSTR)name, MAX_PATH);
		PathRemoveFileSpec((LPSTR)name);
		SetCurrentDirectory((LPSTR)name);
	}

	bool ALT = false;

	bool isKeyDown(int key)
	{
		return GetKeyState(key) & 0x8000;
	}

	bool showTimeFlag = false;

#define CAM_KEY VK_CONTROL

	#include "editorWindow.h"
	#include "hotReload.h"
	#include "uiDraw.h"

	bool isMouseOver(float x, float y, float w, float h)
	{
		if (ui::mousePos.x > x && ui::mousePos.x < x + w &&
			ui::mousePos.y > y &&
			ui::mousePos.y < y + h)
		{
			return true;
		}

		return false;
	}

	#include "timeLine.h"
	#include "viewCam.h"
	#include "paramEdit.h"

	void Init()
	{
		SelfLocate();
		SetRenderWindowPosition();
		ViewCam::Init();
	}

	void ProcessContext()
	{
		if (isKeyDown(CAM_KEY) && isKeyDown(TIME_KEY)) return;

		if (isKeyDown(CAM_KEY) && uiContext != uiContext_::camera)
		{
			ViewCam::storedCamera = ViewCam::currentCamera;
			ui::mouseLastPos = ui::mousePos;
			uiContext = uiContext_::camera;
			return;
		}

		if (!isKeyDown(CAM_KEY) && uiContext == uiContext_::camera)
		{
			ui::mouseLastPos = ui::mousePos;
			paramEdit::yPosLast = paramEdit::yPos;
			uiContext = uiContext_::stack;
			return;
		}

		if (isKeyDown(TIME_KEY) && uiContext != uiContext_::timeline)
		{
			ui::mouseLastPos = ui::mousePos;
			uiContext = uiContext_::timeline;
			return;
		}
		
		if (!isKeyDown(TIME_KEY) && uiContext == uiContext_::timeline)
		{
			ui::mouseLastPos = ui::mousePos;
			paramEdit::yPosLast = paramEdit::yPos;
			uiContext = uiContext_::stack;
			return;
		}
	}

	void Process()
	{
		showTimeFlag = false;
		hilightedCmd = -1;

		ui::mousePos = ui::GetCusorPos();

		if (TimeLine::play) TimeLine::playMode();

		ProcessContext();

		if (ui::mousePos.x < 1 || ui::mousePos.x >= 0 || ui::mousePos.y < 1 || ui::mousePos.x >= 0) {

			ui::mouseDelta.x = ui::mousePos.x - ui::mouseLastPos.x;
			ui::mouseDelta.y = ui::mousePos.y - ui::mouseLastPos.y;
			ui::mouseAngle = -atan2f(ui::mousePos.y - .5f, ui::mousePos.x - .5f);
			ui::mouseAngleDelta = ui::mouseAngle - ui::mouseLastAngle;

			ui::lbDown = isKeyDown(VK_LBUTTON);
			ui::rbDown = isKeyDown(VK_RBUTTON);
			ui::mbDown = isKeyDown(VK_MBUTTON);
			ui::LeftDown = isKeyDown(VK_LEFT);
			ui::RightDown = isKeyDown(VK_RIGHT);
		}

		if (!ui::lbDown)
		{
			lbDragContext = DragContext::free;
		}

		gapi.rt(texture::mainRT);
		gapi.cull(cullmode::off);
		gapi.depth(depthmode::off);

		//if (paramEdit::editContext)
		{
			paramEdit::ShowStack();
		}

		//if (isKeyDown(TIME_KEY) || showTimeFlag)
		{
			TimeLine::ProcessInput();
			TimeLine::Draw();
			paramEdit::CamKeys();
			paramEdit::showTrack();
		}

		ViewCam::setup();
		ViewCam::setCamMat();

		//if (Camera::viewCam.overRide)
		{
			if (isKeyDown(CAM_KEY)|| ViewCam::flyToCam < 1.f)
			{
				paramEdit::ObjHandlers();
				ViewCam::Draw();
			} 
		}

		

		//ViewCam::setCamMat();

	}
}