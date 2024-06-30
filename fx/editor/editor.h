using namespace dx11;

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

	#include "editorWindow.h"
	#include "hotReload.h"
	#include "uiDraw.h"
	#include "timeLine.h"
	#include "viewCam.h"
	#include "paramEdit.h"

	void Init()
	{
		SelfLocate();
		SetRenderWindowPosition();
		ViewCam::Init();
	}

	void Process()
	{
		hilightedCmd = -1;
		paramEdit::editContext = !(isKeyDown(CAM_KEY) | isKeyDown(CAM_KEY2) | isKeyDown(TIME_KEY));

		ui::mousePos = ui::GetCusorPos();

	//	if (ui::mousePos.x > 1 || ui::mousePos.x < 0 || ui::mousePos.y > 1 || ui::mousePos.x < 0) return;

		ui::mouseDelta.x = ui::mousePos.x - ui::mouseLastPos.x;
		ui::mouseDelta.y = ui::mousePos.y - ui::mouseLastPos.y;
		ui::mouseAngle = - atan2f(ui::mousePos.y - .5f, ui::mousePos.x - .5f);
		ui::mouseAngleDelta = ui::mouseAngle - ui::mouseLastAngle;

		ui::lbDown = isKeyDown(VK_LBUTTON) ? true : false;
		ui::rbDown = isKeyDown(VK_RBUTTON) ? true : false;
		ui::mbDown = isKeyDown(VK_MBUTTON) ? true : false;
		ui::LeftDown = isKeyDown(VK_LEFT) ? true : false;
		ui::RightDown = isKeyDown(VK_RIGHT) ? true : false;

		gapi.rt(texture::mainRT);
		gapi.cull(cullmode::off);
		gapi.depth(depthmode::off);

		if (isKeyDown(TIME_KEY))
		{
			TimeLine::Draw();
		}

		if (paramEdit::editContext)
		{
			paramEdit::ShowStack();
		}

		ViewCam::setup();

		if (Camera::viewCam.overRide)
		{
			if (isKeyDown(CAM_KEY)|| isKeyDown(CAM_KEY2) || ViewCam::flyToCam < 1.f)
			{
				paramEdit::ObjHandlers();
				ViewCam::Draw();
			} 
		}

		ViewCam::setCamMat();

	}
}