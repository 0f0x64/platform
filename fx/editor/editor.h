//live wheel param changer
#include <atlbase.h>
#import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("8.0") lcid("0") raw_interfaces_only named_guids
//----

int a = 44;
namespace editor
{

	struct {
		bool newPos = false;
		int line;
		int column;
		char fileName[MAX_PATH];
		float mouseDelta;

		void Update()
		{
			long newline = 0;
			long newcolumn = 0;

			HRESULT result;
			CLSID clsid;
			result = ::CLSIDFromProgID(L"VisualStudio.DTE", &clsid);
			if (FAILED(result))
				return;

			CComPtr<IUnknown> punk;
			result = ::GetActiveObject(clsid, NULL, &punk);
			if (FAILED(result))
				return;

			CComPtr<EnvDTE::_DTE> DTE;
			DTE = punk;

			CComPtr<EnvDTE::ItemOperations> item_ops;
			result = DTE->get_ItemOperations(&item_ops);
			if (FAILED(result))
				return;

			CComPtr<EnvDTE::Document> doc;
			result = DTE->get_ActiveDocument(&doc);
			if (FAILED(result))
				return;

			CComBSTR _fileName;
			doc->get_FullName(&_fileName);
			_bstr_t wrapper(_fileName);
			const char* newFileName = wrapper;

			CComPtr<IDispatch> selection_dispatch;
			result = doc->get_Selection(&selection_dispatch);
			if (FAILED(result))
				return;

			CComPtr<EnvDTE::TextSelection> selection;
			result = selection_dispatch->QueryInterface(&selection);
			if (FAILED(result))
				return;

			EnvDTE::VirtualPoint* pActivePoint = nullptr;
			HRESULT hr = selection->get_ActivePoint(&pActivePoint);

			pActivePoint->get_Line(&newline);
			pActivePoint->get_LineCharOffset(&newcolumn);

			if (line != newline || column != newcolumn || strcmp(fileName, newFileName))
			{
				newPos = true;
				strcpy(fileName, newFileName);
				line = newline;
				column = newcolumn;
			}
			else
			{
				newPos = false;
			}

		}

		void InsertInSmallFile(const std::string& path, size_t pos, size_t pos_end, const std::string& text) {
			// 1. Read the entire file into memory
			std::ifstream in(path, std::ios::binary);
			if (!in) return; // Handle file open error

			std::string contents((std::istreambuf_iterator<char>(in)),
				std::istreambuf_iterator<char>());
			in.close();

			// 2. Validate bounds
			if (pos > contents.size()) pos = contents.size();
			if (pos_end > contents.size()) pos_end = contents.size();
			if (pos_end < pos) pos_end = pos; // Ensure range is valid

			// 3. Replace the range [pos, pos_end) with text
			// This effectively: 
			// - Keeps everything from 0 to pos
			// - Inserts 'text'
			// - Keeps everything from pos_end to the end
			int s = pos;
			int e = pos_end;
			contents.replace(pos, pos_end - pos, text);
			int a = 123;
			// 4. Overwrite the file with the modified content
			std::ofstream out(path, std::ios::binary | std::ios::trunc);
			out << contents;
		}

		char paramStr[100];

		int pStart = 0;
		int pEnd = 0;

		void UpdateParamStr()
		{
			std::ifstream ifile(fileName);
			std::string s;

			int lc = 1;
			pStart = 0;

			if (!ifile.is_open()) return;

			while (true)
			{
				if (!getline(ifile, s))
				{
					ifile.close();
					return;
				}
				if (lc < line) {
					pStart += s.length()+2;
				}

				if (lc == line) break;
				lc++;
			}

			if (s.length() < 1) return;

			int ofs = 0;

			if (!std::isdigit(static_cast<unsigned char>(s[column - 1])) && s[column - 1] != '-')
			{
				if (column - 2 < 0)
				{
					strcpy(paramStr, "nan");
					return;
				}

				if (column - 2 >= 0 && std::isdigit(static_cast<unsigned char>(s[column - 2])) && s[column - 1] != '-')
				{
					ofs = 1;
				}
				else
				{
					strcpy(paramStr, "nan");
					return;
				}
			}

			//search start
			int start = column - 1 - ofs;
			while (true)
			{
				start--;
				if (start < 0) return;

				if (start >= 0 && !std::isdigit(static_cast<unsigned char>(s[start])))
				{
					start++;
					break;
				}
			}

			//search for possible sign
			int signPos = start;
			while (true)
			{
				signPos--;
				if (signPos < 0) break;

				if (signPos >= 0)
				{
					if (s[signPos] == ' ')
					{
						continue;
					}
					else if (s[signPos] == '-')
					{
						break;
					}
				}
			}

			if (signPos >= 0) {
				start = signPos;
			}


			//search end
			int end = column - 1 - ofs;
			while (true)
			{
				end++;
				if (end >= s.length())
				{
					end--;
					break;
				}

				if (!std::isdigit(static_cast<unsigned char>(s[end])))
				{
					//end--;
					break;
				}
			}

			pStart += start;
			pEnd = pStart + (end-start);

			size_t copied = s.copy(paramStr, end - start, start);
			paramStr[copied] = '\0';

			ifile.close();
		}

	} VsTextCurorPos;

}

#include "editor\cmdEditService.h"

#undef regDrawer
#undef reflect
#undef reflect_close
#undef cmd(name, ...)

#define regDrawer(name) track_desc.channel[curChannel].cmdIndex = cmdCounter - 1
#define regfuncGroup(name) strcpy(cmdParamDesc[cmdCounter-1].funcGroup, #name); 
#define REFLECTION true

#if REFLECTION
	#define reflect editor::paramEdit::reflect_f(&in, caller, std::source_location::current())
	#define reflect_close cmdLevel--
	#define cmd(name, ...) struct alignas(1) CAT(name,_params) {__VA_ARGS__}; \
	void name(CAT(name,_params) in ,const std::source_location caller = std::source_location::current())
#endif

bool resize = true;

using namespace dx11;

std::vector<std::string> regex_split(const std::string& str, const std::regex& reg) {

	const std::sregex_token_iterator beg{ str.cbegin(), str.cend(), reg, -1 };
	const std::sregex_token_iterator end{};
	return { beg, end };
}

int getParamByStr(int i, const char* str)
{
	for (int j = 0; j < cmdParamDesc[i].pCount; j++)
	{
		if (!strcmp(cmdParamDesc[i].param[j].name, str))
		{
			return cmdParamDesc[i].param[j].value;
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

	enum class context { free = -1, timeCursor = -2, timeKey = -3, cameraView = -4, cameraButtons = -5, commonUIButtons = -6 };

	bool isFree()
	{
		return dragID.cmd == (int)context::free;
	}

	bool check(int cmd, int param)
	{
		return (cmd == dragID.cmd) && (param == dragID.param);
	}

	bool check(int cmd, const char* paramName)
	{
		auto param = getParamIndexByStr(cmd,paramName);
		return (cmd == dragID.cmd) && (param == dragID.param);
	}

	bool check(context i)
	{
		return ((int)i == dragID.cmd);
	}

	bool check(int i)
	{
		return (i == dragID.cmd);
	}

	void set(context i)
	{
		dragID.cmd = (int)i;
	}

	void set(int i)
	{
		dragID.cmd = i;
	}

	void set(int cmd, int param)
	{
		dragID = { cmd,param };
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

		int storedParam = 0;
		int currentParam = -1;

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

		void pLimits(int cCmd = currentCmd, int cParam = currentParam)
		{
			auto cp = cmdParamDesc[cCmd].param[cParam];
			cmdParamDesc[cCmd].param[cParam].value = clamp(cp.value, cp._min, cp._max);
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
		ui::Init();
		ViewCam::Init();
		TimeLine::screenLeft = .151*1920./dx11::width;
		TimeLine::zoomOut = (int)(TimeLine::timelineLen / (TimeLine::screenRight - TimeLine::screenLeft));
		TimeLine::pos = -TimeLine::ScreenToTime(TimeLine::screenLeft);
	}

	enum class editorMode_ {
		graphics, music
	};

	int editorMode_count = 2;

	editorMode_ editorMode = editorMode_::graphics;

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
		VsTextCurorPos.Update();
		if (VsTextCurorPos.newPos)
		{
			OutputDebugString(VsTextCurorPos.fileName);
			char n[10];
			_itoa(VsTextCurorPos.line, n, 10);
			OutputDebugString(" line:");
			OutputDebugString(n);
			_itoa(VsTextCurorPos.column, n, 10);
			OutputDebugString(" column:");
			OutputDebugString(n);

			OutputDebugString(" param:");
			VsTextCurorPos.UpdateParamStr();
			OutputDebugString(VsTextCurorPos.paramStr);
			OutputDebugString("\n");

		}

		paramsAreLoaded = true;
		paramEdit::top = 0;
		paramEdit::bottom = 1;

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

		Textures::RenderTarget(texture::mainRT);
		Rasterizer::Cull(cullmode::off);
		Depth::Depth(depthmode::off);


		//if (paramEdit::editContext)
		if (editorMode == editorMode_::graphics)
		{
			paramEdit::ShowStack();
		}

		//if (isKeyDown(TIME_KEY) || showTimeFlag)

		{
			TimeLine::ProcessInput();
			TimeLine::Draw();
		}

		if (editorMode == editorMode_::graphics)
		{
			paramEdit::CamKeys();
		}

		if (editorMode == editorMode_::music)
		{
			paramEdit::showTrack();
		}

		ViewCam::setup();
		ViewCam::setCamMat();

		//if (Camera::viewCam.overRide)
		if (editorMode == editorMode_::graphics)
		{
			if (isKeyDown(CAM_KEY)|| ViewCam::flyToCam < 1.f)
			{
				paramEdit::ObjHandlers();
				ViewCam::Draw();
			} 
		}


		if (currentCmd_backup != currentCmd)
		{
			paramEdit::SaveToSource(currentCmd_backup);
			//SetForegroundWindow(vsHWND);
		}

		//ViewCam::setCamMat();
		//ui::Text::Draw(str, tx, ty, th, th);

		//common ui
		char modeText[22];
		switch (editorMode)
		{
		case editorMode_::graphics:
			strcpy(modeText,"graphics");
			break;
		case editorMode_::music:
			strcpy(modeText, "music");
			break;
		}
		
		Rasterizer::Scissors({ 0,0,(float)dx11::width,(float)dx11::height });
		ui::Box::Setup();
		ui::style::Base();
		ui::style::button::hAlign = ui::style::align_h::center;
		ui::style::button::zoom = true;

		if (paramEdit::ButtonPressed(modeText, 0, 0, ui::style::box::width/1.5f, ui::style::box::height/1.2f) && drag.isFree())
		{
			editorMode = (editorMode_)((int)editorMode+1);
			editorMode = (editorMode_)((int)editorMode%editorMode_count);
			
			drag.set(drag.context::commonUIButtons);
			
			currentCmd = -1;
			paramEdit::currentParam = -1;
		}
		

	}

	void SaveAndExit()
	{
		editor::paramEdit::SaveToSource(currentCmd);

		#if vsWindowManagement
				auto rc = editor::primaryRC;
				//SetWindowPos(editor::vsHWND, HWND_TOP, rc.right, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);
				SetWindowPos(editor::vsHWND, HWND_TOP, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);
				ShowWindow(editor::vsHWND, SW_MAXIMIZE);
				UpdateWindow(editor::vsHWND);
		#endif
	}

	void UpdateAspect()
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		dx11::aspect = float(height) / float(width);
		dx11::iaspect = float(width) / float(height);
	}

	void RecompilationCheck(bool &precalc)
	{
		if (codeRecompiled) {
			codeRecompiled = false;
			paramsAreLoaded = false;
			precalc = false;
		}

		cmdLevel = 0;
	}

}