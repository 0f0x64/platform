//live wheel param changer
#include <atlbase.h>
#import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("8.0") lcid("0") raw_interfaces_only named_guids
//----


namespace editor
{

	struct {
		bool newPos = false;
		int line =0;
		int column =0;
		char fileName[MAX_PATH];
		float mouseDelta =0;

		void Update()
		{
			newPos = false;

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
			result = doc->get_FullName(&_fileName);
			if (FAILED(result))
				return;

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
			result = selection->get_ActivePoint(&pActivePoint);
			if (FAILED(result))
				return;

			result = pActivePoint->get_Line(&newline);
			if (FAILED(result))
				return;

			result = pActivePoint->get_LineCharOffset(&newcolumn);
			if (FAILED(result))
				return;


			if (line != newline || column != newcolumn || strcmp(fileName, newFileName))
			{
				newPos = true;
				strcpy(fileName, newFileName);
				line = newline;
				column = newcolumn;
			}
			else
			{
				
			}

		}

		void RestorePos()
		{
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
			result = doc->get_FullName(&_fileName);
			if (FAILED(result))
				return;

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

			selection->MoveToLineAndOffset(line, column, false);
		}

		/*void InsertInSmallFile(const std::string& path, size_t pos, size_t pos_end, const std::string& text) {
			// 1. Open and get size
			std::ifstream in(path, std::ios::binary | std::ios::ate);
			if (!in) return;

			size_t fileSize = static_cast<size_t>(in.tellg());
			in.seekg(0, std::ios::beg);

			// 2. Read entire original file
			std::vector<char> original(fileSize);
			if (fileSize > 0) {
				in.read(original.data(), fileSize);
			}
			in.close();

			// 3. Clamp bounds
			if (pos > fileSize) pos = fileSize;
			if (pos_end > fileSize) pos_end = fileSize;
			if (pos_end < pos) pos_end = pos;

			// 4. Calculate new size: [Prefix] + [New Text] + [Suffix]
			size_t prefixSize = pos;
			size_t suffixSize = fileSize - pos_end;
			size_t newSize = prefixSize + text.size() + suffixSize;

			// 5. Build the new buffer manually
			std::vector<char> finalBuffer;
			finalBuffer.reserve(newSize);

			// Copy Prefix: [0 to pos)
			finalBuffer.insert(finalBuffer.end(), original.begin(), original.begin() + prefixSize);

			// Copy New Text
			finalBuffer.insert(finalBuffer.end(), text.begin(), text.end());

			// Copy Suffix: [pos_end to end)
			finalBuffer.insert(finalBuffer.end(), original.begin() + pos_end, original.end());

			// 6. Atomic-style overwrite
			std::ofstream out(path, std::ios::binary | std::ios::trunc);
			if (out && !finalBuffer.empty()) {
				out.write(finalBuffer.data(), finalBuffer.size());
			}
		}*/

		void InsertInSmallFile(const std::string& path, size_t pos, size_t pos_end, const std::string& text) {
			// 1. Открываем файл и определяем размер одним системным вызовом
			std::ifstream in(path, std::ios::binary | std::ios::ate);
			if (!in) return;

			const size_t fileSize = static_cast<size_t>(in.tellg());

			// Валидация границ (clamping)
			if (pos > fileSize) pos = fileSize;
			if (pos_end > fileSize) pos_end = fileSize;
			if (pos_end < pos) pos_end = pos;

			const size_t suffixSize = fileSize - pos_end;
			const size_t newSize = pos + text.size() + suffixSize;

			// 2. Аллоцируем память ОДИН раз под итоговый размер
			std::vector<char> buffer(newSize);

			// 3. Читаем ПЕРВУЮ часть файла прямо в буфер
			if (pos > 0) {
				in.seekg(0, std::ios::beg);
				in.read(buffer.data(), pos);
			}

			// 4. Копируем текст в середину буфера (из памяти в память)
			if (!text.empty()) {
				std::copy(text.begin(), text.end(), buffer.data() + pos);
			}

			// 5. Читаем ВТОРУЮ часть файла (суффикс) прямо в буфер
			if (suffixSize > 0) {
				in.seekg(pos_end, std::ios::beg);
				in.read(buffer.data() + pos + text.size(), suffixSize);
			}
			in.close();

			// 6. Записываем весь буфер одним куском
			// Используем std::ofstream::write для максимальной скорости
			std::ofstream out(path, std::ios::binary | std::ios::trunc);
			if (out) {
				out.write(buffer.data(), newSize);
			}
		}

		char paramStr[100];

		int pStart = 0;
		int pEnd = 0;

		void UpdateParamStr()
		{
			strcpy(paramStr, "nan");

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

			if (column >= s.length()) return;

			if (s.length() < 1) return;

			if (column -1 < 0) return;

			int ofs = 0;

			if (!std::isdigit(static_cast<unsigned char>(s[column - 1])) && s[column - 1] != '-')
			{
				if (column - 2 < 0) return;

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

	float mouseY = 0;
	int storedVal = 0;
	bool stored = false;
	bool needNewPos = true;

	bool controlParams = false;

	float dTimer = 0;

	int oldRange = 0;

	int GetWindowWidth(HWND hwnd) {
		RECT rect;
		if (GetWindowRect(hwnd, &rect)) {
			return rect.right - rect.left;
		}
		return -1;
	}

	int GetWindowHeight(HWND hwnd) {
		RECT rect;
		if (GetWindowRect(hwnd, &rect)) {
			return rect.bottom - rect.top;
		}
		return -1;
	}

	void updateRange()
	{
		if (!controlParams) return;

		HWND hSlider = GetDlgItem(g_hDlg, 1001);
		int range = 100;
		if (GetAsyncKeyState(VK_CONTROL))
		{
			range *= 10;
		}
		if (GetAsyncKeyState(VK_SHIFT))
		{
			range *= 10;
		}

		if (range != oldRange)
		{
			if (strlen(editor::VsTextCurorPos.paramStr) > 0 && strcmp(editor::VsTextCurorPos.paramStr, "nan"))
			{
				g_SliderValue = strtol(editor::VsTextCurorPos.paramStr, NULL, 10);

				SendMessage(hSlider, TBM_SETRANGE, TRUE, MAKELPARAM(g_SliderValue - range, g_SliderValue + range));
				SendMessage(hSlider, TBM_SETPOS, TRUE, g_SliderValue);
				oldRange = range;

				TCHAR bufMin[24], bufMax[24];
				wsprintf(bufMin, TEXT("%d"), g_SliderValue - range);
				wsprintf(bufMax, TEXT("%d"), g_SliderValue + range);

				SetWindowText(GetDlgItem(g_hDlg, 1003), bufMin);
				SetWindowText(GetDlgItem(g_hDlg, 1004), bufMax);

			}
		}
	}

	bool controlParamsOld = false;
	int pIndex = -1;
	int cmdIndex = -1;

	void calcCmdAndParamIndicies()
	{

		pIndex = -1;
		cmdIndex = -1;

		int ln = -1;

		for (int i = 0; i < cmdCounter; i++)
		{
			char* fn = cmdParamDesc[i].caller.fileName;
			if (!lstrcmp(fn, VsTextCurorPos.fileName))
			{
				if (VsTextCurorPos.line >= cmdParamDesc[i].caller.line)
				{
					ln = max(ln, cmdParamDesc[i].caller.line);
					cmdIndex = i;
				}
			}
		}
		if (ln > 0)
		{
			std::ifstream ifile(VsTextCurorPos.fileName);
			std::string s;

			int lc = 1;

			if (!ifile.is_open()) return;

			//find command start
			while (true)
			{
				if (!getline(ifile, s))
				{
					ifile.close();
					return;
				}

				if (lc == ln) break;
				lc++;
			}

			//find 
			std::string sp = s;
			s.clear();

			while (true)
			{
				size_t end = sp.find(";");
				if (end != std::string::npos)
				{
					sp.erase(end);
				}

				if (lc == VsTextCurorPos.line)
				{
					sp.erase(min(VsTextCurorPos.column, sp.length()));
					s += sp;
					break;
				}

				s += sp;
				s += "\n";

				if (!getline(ifile, sp))
				{
					ifile.close();
					return;
				}

				lc++;
			}


			size_t start = s.find(cmdParamDesc[cmdIndex].funcName);

			ifile.close();

			pIndex = 0;
			for (int i = 0; i < s.length(); i++)
			{
				if (s[i] == ',') pIndex++;
			}
		}
	}

	void Process()
	{
		

		ui::mousePos = ui::GetCusorPos();
		paramsAreLoaded = true;



		if ((GetAsyncKeyState(VK_MBUTTON)||
			 GetAsyncKeyState(VK_ESCAPE)) 
			&& timer::frameBeginTime-dTimer >200 &&
			(GetForegroundWindow() == vsHWND || 
				GetForegroundWindow() == g_hDlg)
				)
		{
			controlParams = !controlParams;
			dTimer = timer::frameBeginTime;
			POINT p;

			GetCursorPos(&p);
			SetWindowPos(g_hDlg, NULL, p.x- GetWindowWidth(g_hDlg) /2, p.y - GetWindowHeight(g_hDlg)/2, 0, 0, SWP_NOSIZE);
		}

		if (controlParams)
		{
			//controlParams = true;
			if (controlParamsOld != controlParams)
			{
				VsTextCurorPos.Update();
				if (VsTextCurorPos.newPos)
				{
					VsTextCurorPos.UpdateParamStr();

					if (strlen(editor::VsTextCurorPos.paramStr) > 0 && strcmp(editor::VsTextCurorPos.paramStr, "nan"))
					{
						calcCmdAndParamIndicies();
						
						controlParamsOld = controlParams;
						needNewPos = false;
						long val = strtol(editor::VsTextCurorPos.paramStr, NULL, 10);
						g_SliderValue = val;

						HWND hSlider = GetDlgItem(g_hDlg, 1001);
						updateRange();
						SendMessage(hSlider, TBM_SETPOS, TRUE, g_SliderValue);
						SetDlgItemInt(g_hDlg, 1002, g_SliderValue, TRUE);
						ShowWindow(g_hDlg, SW_SHOW);
					}
					else
					{
						controlParams = false;
					}
				}
			}
		}
		else
		{
			if (controlParamsOld != controlParams)
			{
				ShowWindow(g_hDlg, SW_HIDE);
				//SetFocus(vsHWND);
				//SetActiveWindow(vsHWND);
				SetForegroundWindow(vsHWND);
				controlParamsOld = controlParams;
				oldRange = 0;
				//editor::VsTextCurorPos.RestorePos();
			}
		}

		wheelMode = controlParams;

		if (!controlParams)
		{
			needNewPos = true;
		}
		else 
		{
			HWND hSlider = GetDlgItem(g_hDlg, 1001);
			updateRange();

			if (strlen(editor::VsTextCurorPos.paramStr) > 0 && strcmp(editor::VsTextCurorPos.paramStr, "nan"))
			{
				long val = strtol(editor::VsTextCurorPos.paramStr, NULL, 10);

				if (val != g_SliderValue)
				{
					val = g_SliderValue;

					SetDlgItemInt(g_hDlg, 1002, val, TRUE);
					//SendMessage(hSlider, TBM_SETPOS, TRUE, g_SliderValue);

					char modified[100];
					_itoa(val, modified, 10);
					bool lenChanged = false;
					if (strlen(editor::VsTextCurorPos.paramStr) != strlen(modified)) lenChanged = true;

					strcpy(editor::VsTextCurorPos.paramStr, modified);
					editor::VsTextCurorPos.InsertInSmallFile(editor::VsTextCurorPos.fileName, editor::VsTextCurorPos.pStart, editor::VsTextCurorPos.pEnd, modified);

					if (lenChanged)
					{
						VsTextCurorPos.UpdateParamStr();
					}
					//editor::VsTextCurorPos.RestorePos();
					//paramsAreLoaded = false;

					


						if (cmdIndex >= 0 && pIndex >= 0)
						{
							cmdParamDesc[cmdIndex].param[pIndex].value = val;
							strcpy(cmdParamDesc[cmdIndex].param[pIndex].strValue, modified);
						}

						//char aa[10];
						//_itoa(pIndex, aa, 10);
						//OutputDebugString(aa);
						//OutputDebugString("\n");

						int a = 0;
					

				}
			}
		}

				
		paramEdit::top = 0;
		paramEdit::bottom = 1;

		currentCmd_backup = currentCmd;

		showTimeFlag = false;
		hilightedCmd = -1;



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