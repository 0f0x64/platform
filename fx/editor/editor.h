using namespace dx11;

std::vector<std::string> regex_split(const std::string& str, const std::regex& reg) {
	const std::sregex_token_iterator beg{ str.cbegin(), str.cend(), reg, -1 };
	const std::sregex_token_iterator end{};

	return { beg, end };
}

enum class DragContext : int { free, timeCursor, timeKey, cameraView, cameraButtons };
DragContext lbDragContext = DragContext::free;

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



#define CAM_KEY VK_CONTROL
#define CAM_KEY2 VK_SHIFT

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

	void Process()
	{
		hilightedCmd = -1;
		paramEdit::editContext = !(isKeyDown(CAM_KEY) | isKeyDown(CAM_KEY2) | isKeyDown(TIME_KEY));

		ui::mousePos = ui::GetCusorPos();

		ui::mouseDelta.x = ui::mousePos.x - ui::mouseLastPos.x;
		ui::mouseDelta.y = ui::mousePos.y - ui::mouseLastPos.y;
		ui::mouseAngle = - atan2f(ui::mousePos.y - .5f, ui::mousePos.x - .5f);
		ui::mouseAngleDelta = ui::mouseAngle - ui::mouseLastAngle;

		ui::lbDown = isKeyDown(VK_LBUTTON);
		ui::rbDown = isKeyDown(VK_RBUTTON);
		ui::mbDown = isKeyDown(VK_MBUTTON);
		ui::LeftDown = isKeyDown(VK_LEFT);
		ui::RightDown = isKeyDown(VK_RIGHT);

		if (!ui::lbDown)
		{
			lbDragContext = DragContext::free;
		}

		gapi.rt(texture::mainRT);
		gapi.cull(cullmode::off);
		gapi.depth(depthmode::off);

		if (isKeyDown(TIME_KEY) || editor::TimeLine::play)
		{
		
			paramEdit::CamKeys();
			TimeLine::Draw();
			TimeLine::ProcessInput();
			
		}

		if (paramEdit::editContext)
		{
			paramEdit::ShowStack();
		}

		ViewCam::setup();

		ViewCam::setCamMat();

		//if (Camera::viewCam.overRide)
		{
			if (isKeyDown(CAM_KEY)|| isKeyDown(CAM_KEY2) || ViewCam::flyToCam < 1.f)
			{
				
				paramEdit::ObjHandlers();
				ViewCam::Draw();
				
			} 
		}

		

		//ViewCam::setCamMat();

	}
}