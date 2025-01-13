using namespace dx11;



std::vector<std::string> regex_split(const std::string& str, const std::regex& reg) {

	const std::sregex_token_iterator beg{ str.cbegin(), str.cend(), reg, -1 };
	const std::sregex_token_iterator end{};
	return { beg, end };
}

int getParamByStr(int i, const char* str, int index = 0)
{
	for (int j = 0; j < cmdParamDesc[i].pCount; j++)
	{
		if (!strcmp(cmdParamDesc[i].param[j].name, str))
		{
			return cmdParamDesc[i].param[j].value[index];
		}
	}

	return MAXINT;
}

int getParamIndexByStr(int i, const char* str)
{
	for (int j = 0; j < cmdParamDesc[i].pCount; j++)
	{
		if (!strcmp(cmdParamDesc[i].param[j].name, str))
		{
			return j;
		}
	}

	return MAXINT;
}

class {

private:

	struct {
		int cmd;
		int param;
		int subparam;
	} dragID;

public:

	enum class context { free = -1, timeCursor = -2, timeKey = -3, cameraView = -4, cameraButtons = -5 };

	bool isFree()
	{
		return dragID.cmd == (int)context::free;
	}

	bool check(int cmd, int param, int subparam)
	{
		return (cmd == dragID.cmd) && (param == dragID.param) && (subparam == dragID.subparam);
	}

	bool check(int cmd, const char* paramName, int subparam = 0)
	{
		auto param = getParamIndexByStr(cmd,paramName);
		return (cmd == dragID.cmd) && (param == dragID.param) && (subparam == dragID.subparam);
	}

	bool check(context i)
	{
		return ((int)i == dragID.cmd);
	}

	void set(context i)
	{
		dragID.cmd = (int)i;
	}

	void set(int cmd, int param, int subparam)
	{
		dragID = { cmd,param,subparam };
	}

	void setFree()
	{
		dragID.cmd = (int)context::free;
	}

} drag;


enum class uiContext_ : int { stack, camera, timeline, undefined };
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
		if (ui::mousePos.x < 0 || ui::mousePos.x >= 1 ||
			ui::mousePos.y < 0 || ui::mousePos.y >= 1)
		{
			return false;
		}


		if (ui::mousePos.x > x && ui::mousePos.x < x + w &&
			ui::mousePos.y > y && ui::mousePos.y < y + h)
		{
			return true;
		}

		return false;
	}

	namespace paramEdit {

		int floatConvertDigits = 5;

		float yPos = 0.05;
		float yPosLast;

		int storedParam[4] = { 0,0,0,0 };
		int currentParam = -1;
		int subParam = 0;

		bool action = false;
		bool clickOnEmptyPlace;

		float valueDrawOffset = .1f;
		float enumDrawOffset = .2f;
		float enumPos = 0;

		int cursorPos = 0;
		float cursorX = 0;
		float cursorY = 0;
		bool showCursor = false;

		int pCountV = 0;
		float tabLen = 0.f;

		float vScroll = false;

		void rbDown()
		{
			yPosLast = yPos;
		}

		bool saveMe = false;

		float top;
		float bottom;
		float lead;
		float insideX;
		float insideY;
		float x;
		float y;

		float selYpos = 0.5;

		void pLimits(int cCmd = currentCmd, int cParam = currentParam, int cSubParam = subParam)
		{
			auto cp = cmdParamDesc[cCmd].param[cParam];
			cmdParamDesc[cCmd].param[cParam].value[cSubParam] = clamp(cp.value[cSubParam], cp._min, cp._max);
		}

	}

	#include "timeLine.h"
	#include "viewCam.h"
	#include "textEditor.h"
	#include "paramEdit.h"
	#include "trackerUI.h"

	void Init()
	{
		SelfLocate();
		SetRenderWindowPosition();
		ViewCam::Init();
	}

	void ProcessContext()
	{
		if (ui::mousePos.x > 1 || ui::mousePos.x < 0 || ui::mousePos.y > 1 || ui::mousePos.x < 0)
		{
			if (uiContext == uiContext_::camera)
			{
				if (!isKeyDown(CAM_KEY) || !(ui::lbDown|| ui::rbDown))
				{
					uiContext = uiContext_::undefined;
				}
			}

			if (uiContext == uiContext_::timeline)
			{
				if (!isKeyDown(TIME_KEY) || !(ui::lbDown || ui::rbDown))
				{
					uiContext = uiContext_::undefined;
				}
			}

			return;
		}

		if (isKeyDown(CAM_KEY) && isKeyDown(TIME_KEY))
		{
			return;
		}

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
		currentCmd_backup = currentCmd;

		showTimeFlag = false;
		hilightedCmd = -1;

		ui::mousePos = ui::GetCusorPos();

		if (TimeLine::play) TimeLine::playMode();

		ProcessContext();

			ui::mouseDelta.x = ui::mousePos.x - ui::mouseLastPos.x;
			ui::mouseDelta.y = ui::mousePos.y - ui::mouseLastPos.y;
			ui::mouseAngle = -atan2f(ui::mousePos.y - .5f, ui::mousePos.x - .5f);
			ui::mouseAngleDelta = ui::mouseAngle - ui::mouseLastAngle;

			ui::lbDown = isKeyDown(VK_LBUTTON);
			ui::rbDown = isKeyDown(VK_RBUTTON);
			ui::mbDown = isKeyDown(VK_MBUTTON);
			ui::LeftDown = isKeyDown(VK_LEFT);
			ui::RightDown = isKeyDown(VK_RIGHT);
		

		if (!ui::lbDown)
		{
			drag.setFree();
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

		
		if (currentCmd_backup != currentCmd)
		{
			paramEdit::Save(currentCmd_backup);
		}
		//ViewCam::setCamMat();

	}
}