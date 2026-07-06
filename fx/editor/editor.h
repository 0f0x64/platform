//live wheel param changer
#include <atlbase.h>
#import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("8.0") lcid("0") raw_interfaces_only named_guids
//----
#include <cctype>
#include <cstring>
#include <filesystem>
#include <libloaderapi.h>
#include <shlwapi.h>
#include <uxtheme.h>
#pragma comment(lib, "Shlwapi.lib")


namespace editor
{

	bool controlParamsOld = false;
	int pIndex = -1;
	int cmdIndex = -1;
	bool calcCmdAndParamIndicies();

	struct {
		long line =0;
		long column =0;
		char fileName[MAX_PATH];

		char funcName[256];
		char paramStr[1000];

		long pStart = 0;
		long pEnd = 0;

		long start=0;
		long end = 0;

		int slider = 0;
		enum class textType {nonEditable, number,enumList,functionCall, text} typeUnderCursor;
		char typeName[256];
		char textValue[256];

		bool pointBefore = false;

		char* extractNumberAtC(const char* str, size_t pos) {

			pointBefore = false;

			if (!str) return nullptr;
			size_t n = strlen(str);
			if (pos > n) return nullptr;

			int start_ = (int)pos;
			int end_ = (int)pos;

			// 1. Если курсор на пробеле или знаке, ищем цифру вплотную
			if (!std::isdigit((unsigned char)str[pos])) {
				if (pos < n && std::isdigit((unsigned char)str[pos + 1])) { start_ = end_ = (int)pos + 1; }
				else if (pos > 0 && std::isdigit((unsigned char)str[pos - 1])) { start_ = end_ = (int)pos - 1; }
				else if (str[pos] != '-' && str[pos] != '+') return nullptr;
			}

			// 2. Расширяемся вправо (собираем все цифры)
			while (end_ + 1 < (int)n && std::isdigit((unsigned char)str[end_ + 1])) {
				end_++;
			}

			// 3. Расширяемся влево (собираем все цифры)
			while (start_ > 0 && std::isdigit((unsigned char)str[start_ - 1])) {
				start_--;
			}

			// 4. Ищем знак перед числом (с учетом пробелов)
			int signPos = start_ - 1;
			while (signPos >= 0 && std::isspace((unsigned char)str[signPos])) {
				signPos--;
			}

			if (signPos >= 0 && (str[signPos] == '-' || str[signPos] == '+')) {
				start_ = signPos;
			}

			// 5. Сохраняем глобальные оффсеты
			start = (size_t)start_;
			end = (size_t)end_ + 1; // Конец диапазона (не включая символ)
			size_t len = end - start;

			// 6. Копируем результат
			char* result = (char*)malloc(len + 1);
			if (result) {
				memcpy(result, str + start, len);
				result[len] = '\0';
			}

			if (start > 0)
			{
				if (str[start-1] == '.') pointBefore = true;
			}
			

			return result;
		}



		bool is_id_char(char c) {
			return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
		}

		bool isText(const char* str, int pos) {
			if (!str || pos < 0) return false;

			int left = -1;
			int right = -1;

			// 1. Ищем ближайшую кавычку СЛЕВА от pos
			for (int i = pos; i >= 0; --i) {
				if (str[i] == '\"') {
					left = i;
					break;
				}
			}

			// 2. Ищем ближайшую кавычку СПРАВА от pos
			for (int i = pos; str[i] != '\0'; ++i) {
				if (str[i] == '\"') {
					right = i;
					break;
				}
			}

			// Проверяем, что мы действительно МЕЖДУ двумя кавычками
			// (left < pos < right)
			if (left != -1 && right != -1 && left < right && pos > left && pos < right) {
				int length = right - left - 1;

				// Защита от переполнения глобального буфера (255 символов + '\0')
				if (length > 255) length = 255;

				// Копируем текст из основной строки в глобальный буфер
				memcpy(textValue, &str[left + 1], length);
				textValue[length] = '\0'; // Закрываем строку нулем

				return true;
			}

			// Если не в тексте, очищаем глобальный буфер (по желанию)
			textValue[0] = '\0';
			return false;
		}

		bool isEnum(const char* str, int pos) {

			typeName[0] = '\0';

			if (!str || pos < 0) return false;
			int length = static_cast<int>(std::strlen(str));

			// КОРРЕКЦИЯ: Если курсор стоит ПОСЛЕ слова, смещаемся на 1 символ влево, 
			// чтобы "захватить" идентификатор.
			int checkPos = pos;
			if (checkPos >= length || (!is_id_char(str[checkPos]) && checkPos > 0)) {
				if (is_id_char(str[checkPos - 1])) {
					checkPos--;
				}
			}

			// Если даже после коррекции мы не на букве/цифре — значит рядом нет слова
			if (checkPos < 0 || !is_id_char(str[checkPos])) return false;

			// 1. Определяем границы слова вокруг скорректированной позиции
			int idxStart = checkPos;
			while (idxStart >= 0 && is_id_char(str[idxStart])) idxStart--;
			idxStart++;

			int idxEnd = checkPos;
			while (idxEnd < length && is_id_char(str[idxEnd])) idxEnd++;

			// Валидация: не начинается с цифры
			if (std::isdigit(static_cast<unsigned char>(str[idxStart]))) return false;

			// 2. Ищем "::" слева от idxStart
			int i = idxStart - 1;
			while (i >= 1 && std::isspace(static_cast<unsigned char>(str[i]))) i--;

			if (i < 1 || str[i] != ':' || str[i - 1] != ':') return false;

			int opPos = i - 1;
			int typeEnd = opPos - 1;
			while (typeEnd >= 0 && std::isspace(static_cast<unsigned char>(str[typeEnd]))) typeEnd--;
			if (typeEnd < 0 || !is_id_char(str[typeEnd])) return false;

			int typeStart = typeEnd;
			while (typeStart >= 0 && is_id_char(str[typeStart])) typeStart--;
			typeStart++;
			if (std::isdigit(static_cast<unsigned char>(str[typeStart]))) return false;

			int typeLen = typeEnd - typeStart + 1;
			if (typeLen >= (int)sizeof(typeName)) typeLen = sizeof(typeName) - 1;
			std::strncpy(typeName, str + typeStart, typeLen);
			typeName[typeLen] = '\0';

			start = idxStart;
			end = idxEnd;
			return true;
		}



		bool isFunctionCall(const char* str, int pos) {

			funcName[0] = '\0';

			if (!str || pos < 0) return false;
			int length = static_cast<int>(std::strlen(str));

			// 1. Корректируем позицию, если курсор стоит сразу после слова или перед ({
			int checkPos = pos;
			if (checkPos >= length || (!is_id_char(str[checkPos]) && checkPos > 0)) {
				if (is_id_char(str[checkPos - 1])) {
					checkPos--;
				}
			}

			if (checkPos < 0 || !is_id_char(str[checkPos])) return false;

			// 2. Находим границы потенциального имени функции
			int idxStart = checkPos;
			while (idxStart >= 0 && is_id_char(str[idxStart])) idxStart--;
			idxStart++;

			int idxEnd = checkPos;
			while (idxEnd < length && is_id_char(str[idxEnd])) idxEnd++;
			idxEnd--;

			if (idxStart > idxEnd || std::isdigit(static_cast<unsigned char>(str[idxStart]))) return false;

			// 3. Проверка маркера СПРАВА: "({", пропуская пробелы
			int right = idxEnd + 1;
			while (right < length && std::isspace(static_cast<unsigned char>(str[right]))) right++;

			if (right >= length - 1 || str[right] != '(' || str[right + 1] != '{') {
				return false;
			}

			// 4. Проверка маркера СЛЕВА: пробел, "::", начало строки или оператор
			int left = idxStart - 1;
			bool leftValid = false;

			if (left < 0) {
				leftValid = true; // Начало строки - валидно
			}
			else {
				// Пропускаем пробелы слева
				while (left >= 0 && std::isspace(static_cast<unsigned char>(str[left]))) left--;

				if (left < 0) {
					leftValid = true;
				}
				else if (std::isspace(static_cast<unsigned char>(str[idxStart - 1]))) {
					leftValid = true; // Был пробел
				}
				else if (left >= 1 && str[left] == ':' && str[left - 1] == ':') {
					leftValid = true; // Был оператор ::
				}
				else if (std::ispunct(static_cast<unsigned char>(str[left]))) {
					leftValid = true; // Любой знак пунктуации (напр. '=' или ';')
				}
			}

			if (!leftValid) return false;

			// 5. Экстракция имени в глобальную переменную
			int nameLen = idxEnd - idxStart + 1;
			if (nameLen >= (int)sizeof(funcName)) nameLen = sizeof(funcName) - 1;

			std::strncpy(funcName, str + idxStart, nameLen);
			funcName[nameLen] = '\0';

			start = idxStart;
			end = idxEnd;

			return true;
		}

		int GetParamIndex()
		{
			HRESULT result;
			CLSID clsid;
			result = ::CLSIDFromProgID(L"VisualStudio.DTE.17.0", &clsid);
			if (FAILED(result))
				return false;

			CComPtr<IUnknown> punk;
			result = ::GetActiveObject(clsid, NULL, &punk);
			if (FAILED(result))
				return false;

			CComPtr<EnvDTE::_DTE> DTE;
			DTE = punk;

			CComPtr<EnvDTE::ItemOperations> item_ops;
			result = DTE->get_ItemOperations(&item_ops);
			if (FAILED(result))
				return false;

			CComPtr<EnvDTE::Document> doc;
			result = DTE->get_ActiveDocument(&doc);
			if (FAILED(result))
				return false;

			CComPtr<IDispatch> pSelectionObj;
			doc->get_Selection(&pSelectionObj);
			CComPtr<EnvDTE::TextSelection> pSelection;
			pSelectionObj->QueryInterface(&pSelection);

			// 2. Создаем точку для навигации
			CComPtr<EnvDTE::VirtualPoint> pVirtualPoint;
			pSelection->get_ActivePoint(&pVirtualPoint);

			// 2. Convert VirtualPoint to EditPoint to allow CharLeft/GetText operations
			CComPtr<EnvDTE::EditPoint> pPoint;
			pVirtualPoint->CreateEditPoint(&pPoint);

			int commaCount = 0;
			VARIANT_BOOL isAtStart = VARIANT_FALSE;

			// 3. Двигаемся назад до начала документа или символа '{'
			while (true) {
				pPoint->get_AtStartOfDocument(&isAtStart);
				if (isAtStart == VARIANT_TRUE) return -1;

				// Сдвигаемся на 1 символ влево
				pPoint->CharLeft(1);

				// Получаем символ справа от точки (тот, который мы только что "перешагнули")
				BSTR bstrChar = nullptr;
				HRESULT hr = pPoint->GetText(CComVariant(1), &bstrChar);

				if (SUCCEEDED(hr) && bstrChar) {
					wchar_t c = bstrChar[0];
					SysFreeString(bstrChar);

					if (c == L'{') {
						break; // Stop at the opening brace
					}
					if (c == L',') {
						commaCount++;
					}
				}
				else {
					return -1;
				}
			}

			return commaCount;
		}

		bool Update(bool forceToNumStart)
		{
			HRESULT result;
			CLSID clsid;
			result = ::CLSIDFromProgID(L"VisualStudio.DTE.17.0", &clsid);
			if (FAILED(result))
				return false;

			CComPtr<IUnknown> punk;
			result = ::GetActiveObject(clsid, NULL, &punk);
			if (FAILED(result))
				return false;

			CComPtr<EnvDTE::_DTE> DTE;
			DTE = punk;

			CComPtr<EnvDTE::ItemOperations> item_ops;
			result = DTE->get_ItemOperations(&item_ops);
			if (FAILED(result))
				return false;

			CComPtr<EnvDTE::Document> doc;
			result = DTE->get_ActiveDocument(&doc);
			if (FAILED(result))
				return false;

			CComBSTR _fileName;
			result = doc->get_FullName(&_fileName);
			if (FAILED(result))
				return false;

			_bstr_t wrapper(_fileName);
			strcpy(fileName, wrapper);

			CComPtr<IDispatch> selection_dispatch;
			result = doc->get_Selection(&selection_dispatch);
			if (FAILED(result))
				return false;

			CComPtr<EnvDTE::TextSelection> selection;
			result = selection_dispatch->QueryInterface(&selection);
			if (FAILED(result))
				return false;

			EnvDTE::VirtualPoint* pActivePoint = nullptr;
			result = selection->get_ActivePoint(&pActivePoint);
			if (FAILED(result))
				return false;

			result = pActivePoint->get_Line(&line);
			if (FAILED(result))
				return false;

			result = pActivePoint->get_LineCharOffset(&column);
			if (FAILED(result))
				return false;

			//get string under cursor
			CComPtr<IDispatch> pDocDisp;
			result = doc->Object(CComBSTR("TextDocument"), &pDocDisp);
			if (FAILED(result))
				return false;

			CComQIPtr<EnvDTE::TextDocument> pTextDoc(pDocDisp);
			long currentLine = 0;
			result = selection->get_CurrentLine(&currentLine);
			if (FAILED(result))
				return false;

			CComPtr<EnvDTE::EditPoint> pEditPoint;
			result = pTextDoc->CreateEditPoint(NULL, &pEditPoint);
			if (FAILED(result))
				return false;

			result = pEditPoint->MoveToLineAndOffset(currentLine, 1);
			if (FAILED(result))
				return false;

			CComBSTR bstrLineText;
			result = pEditPoint->GetLines(currentLine, currentLine + 1, &bstrLineText);
			if (FAILED(result))
				return false;

			_bstr_t wrapper2(bstrLineText);
			const char* str = wrapper2;
			std::string s = str;

			//calc offset
			CComPtr<EnvDTE::EditPoint> startT;
			result = pTextDoc->CreateEditPoint(NULL, &startT);
			if (FAILED(result))
				return false;

			CComBSTR bstrTextBefore;
			CComVariant varEndPoint(pActivePoint);
			result = startT->GetText(varEndPoint, &bstrTextBefore);
			if (FAILED(result))
				return false;

			int byteOffset = WideCharToMultiByte(CP_UTF8, 0, bstrTextBefore, -1, NULL, 0, NULL, NULL);
			pStart = byteOffset-column;
			
			//search num
			strcpy(paramStr, "nan");
			typeUnderCursor = textType::nonEditable;

			if (isText(s.c_str(), column - 1))
			{
				typeUnderCursor = textType::text;
				return true;
			}

			char* number = extractNumberAtC(s.c_str(), column-1);
			if (number)
			{
				strcpy(paramStr, number);
				free(number);
				typeUnderCursor = textType::number;
			}

			if (isFunctionCall(s.c_str(), column - 1))
			{
				typeUnderCursor = textType::functionCall;
				return true;
			}

			if (isEnum(s.c_str(), column - 1))
			{
				typeUnderCursor = textType::enumList;
				return true;
			}

			if (typeUnderCursor == textType::nonEditable)
			{
				return false;
			}

			column = start+1;

			if (forceToNumStart)
			{
				selection->MoveToLineAndOffset(line, column, VARIANT_FALSE);
			}

			pStart += start;
			pEnd = pStart + (end - start);

			size_t copied = s.copy(paramStr, end - start, start);
			paramStr[copied] = '\0';

			if (strlen(paramStr) <= 0 && !strcmp(paramStr, "nan")) 
				return false;

			return true;

		}

		bool ReplaceTextInVS(const char* text,bool forceToNumStart)
		{
			HRESULT result;
			CLSID clsid;
			result = ::CLSIDFromProgID(L"VisualStudio.DTE.17.0", &clsid);
			if (FAILED(result))
				return false;

			CComPtr<IUnknown> punk;
			result = ::GetActiveObject(clsid, NULL, &punk);
			if (FAILED(result))
				return false;

			CComPtr<EnvDTE::_DTE> DTE;
			DTE = punk;

			CComPtr<EnvDTE::ItemOperations> item_ops;
			result = DTE->get_ItemOperations(&item_ops);
			if (FAILED(result))
				return false;

			CComPtr<EnvDTE::Document> doc;
			result = DTE->get_ActiveDocument(&doc);
			if (FAILED(result))
				return false;

			CComPtr<IDispatch> selection_dispatch;
			result = doc->get_Selection(&selection_dispatch);
			if (FAILED(result))
				return false;

			CComPtr<EnvDTE::TextSelection> selection;
			result = selection_dispatch->QueryInterface(&selection);
			if (FAILED(result))
				return false;

			CComPtr<IDispatch> pDocDisp;
			result = doc->Object(CComBSTR("TextDocument"), &pDocDisp);
			if (FAILED(result))
				return false;

			CComQIPtr<EnvDTE::TextDocument> pTextDoc(pDocDisp);

			CComPtr<EnvDTE::EditPoint> pEditStart;
			pTextDoc->CreateEditPoint(NULL, &pEditStart);
			pEditStart->MoveToLineAndOffset(line, start+1);

			CComPtr<EnvDTE::EditPoint> pEditEnd;
			pTextDoc->CreateEditPoint(NULL, &pEditEnd);
			pEditEnd->MoveToLineAndOffset(line, end + 1);

			CComBSTR bstrNewText(text);
			pEditStart->ReplaceText(CComVariant(pEditEnd), bstrNewText, (long)EnvDTE::vsEPReplaceTextOptions::vsEPReplaceTextKeepMarkers);

			if (forceToNumStart)
			{
				selection->MoveToLineAndOffset(line, start + 1, VARIANT_FALSE);
			}

			return true;
		}

		bool haveChanges()
		{

			HRESULT result;
			CLSID clsid;
			result = ::CLSIDFromProgID(L"VisualStudio.DTE.17.0", &clsid);
			if (FAILED(result))
				return false;

			CComPtr<IUnknown> punk;
			result = ::GetActiveObject(clsid, NULL, &punk);
			if (FAILED(result))
				return false;

			CComPtr<EnvDTE::_DTE> DTE;
			DTE = punk;

			CComPtr<EnvDTE::ItemOperations> item_ops;
			result = DTE->get_ItemOperations(&item_ops);
			if (FAILED(result))
				return false;

			CComPtr<EnvDTE::Document> doc;
			result = DTE->get_ActiveDocument(&doc);
			if (FAILED(result))
				return false;

			VARIANT_BOOL isSaved;
			doc->get_Saved(&isSaved);

			if (isSaved == VARIANT_FALSE) {

				return true;
			}

			return false;

		}

		bool saveChanges()
		{

			HRESULT result;
			CLSID clsid;
			result = ::CLSIDFromProgID(L"VisualStudio.DTE.17.0", &clsid);
			if (FAILED(result))
				return false;

			CComPtr<IUnknown> punk;
			result = ::GetActiveObject(clsid, NULL, &punk);
			if (FAILED(result))
				return false;

			CComPtr<EnvDTE::_DTE> DTE;
			DTE = punk;

			CComPtr<EnvDTE::ItemOperations> item_ops;
			result = DTE->get_ItemOperations(&item_ops);
			if (FAILED(result))
				return false;

			CComPtr<EnvDTE::Document> doc;
			result = DTE->get_ActiveDocument(&doc);
			if (FAILED(result))
				return false;

			VARIANT_BOOL isSaved;
			doc->get_Saved(&isSaved);

			if (isSaved == VARIANT_FALSE) {

				CComBSTR emptyPath(L"");
				EnvDTE::vsSaveStatus status;
				doc->Save(emptyPath, &status);

				return true;
			}

			return false;

		}


	} VsEditor;

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

	#define cmd(name, ...) _Pragma("pack(push, 1)") struct CAT(name,_params) { FOR_EACH(SEMI, __VA_ARGS__) }; _Pragma("pack(pop)") \
    void name(CAT(name,_params) in, const std::source_location caller = std::source_location::current())

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
	#include "paramEdit.h"
	#include "trackerUI.h"

	void Init()
	{
		SelfLocate();
		ui::Init();
		ViewCam::Init();
		//TimeLine::screenLeft = .151*1920./dx11::width;
		TimeLine::screenLeft = 0;
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

	bool fileIsShader()
	{
		return strstr(VsEditor.fileName, dx11::Shaders::shaderExtension);
	}

	bool calcCmdAndParamIndicies()
	{
		//TODO: add check isField (.name = val, format or ({,,,}) format)

		pIndex = -1;
		cmdIndex = -1;

		if (fileIsShader()) return false;

		int ln = -1;

		for (int i = 0; i < editor::paramEdit::registry.size(); i++)
		{
			char* fn = cmdParamDesc[i].caller.fileName;
			if (strcmp(fn, VsEditor.fileName) == 0)
			{
				if (cmdParamDesc[i].caller.line <= VsEditor.line &&
					cmdParamDesc[i].caller.endLine >= VsEditor.line)
				{
					//if (cmdParamDesc[i].caller.line > ln)
					{
						//ln = cmdParamDesc[i].caller.line;
						cmdIndex = i;
						currentCmd = i;

					}
		
				}
			}
		}



		if (cmdIndex < 0) return false;

		if (VsEditor.typeUnderCursor != VsEditor.textType::functionCall)
		{
			pIndex = VsEditor.GetParamIndex();
		}

		if (pIndex < 0) return false;

		return true;
		
	}

	namespace fs = std::filesystem;

	void getPathParts(const std::string& fullPath, std::string& nameOnly, std::string& parentFolder) {
		fs::path p(fullPath);

		// 1. Имя файла без расширения (stem)
		nameOnly = p.stem().string();

		// 2. Название папки на один уровень выше
		// p.parent_path() возвращает полный путь к папке, 
		// .filename() извлекает только последнее имя в этом пути
		parentFolder = p.parent_path().filename().string();
	}

	void recompileShader()
	{
		std::string name, folder;

		getPathParts(VsEditor.fileName, name, folder);

		std::string rPath = "\/" + folder + "\/" + name + dx11::Shaders::shaderExtension;

		// detect vertex/pixel shader and slot
		if (folder[0] == 'v')
		{
			int i = 0;
			while (i < dx11::Shaders::vsCount)
			{
				if (!strcmp(dx11::Shaders::vsList[i], name.c_str()))
				{
					dx11::Shaders::CreateVS(i, rPath.c_str(),true);
					break;
				}
				i++;
			}

		}

		if (folder[0] == 'p')
		{
			int i = 0;
			while (i < dx11::Shaders::psCount)
			{
				if (!strcmp(dx11::Shaders::psList[i], name.c_str()))
				{
					dx11::Shaders::CreatePS(i, rPath.c_str(),true);
					break;
				}
				i++;
			}

		}
	}

	int oldValue = 0;
	int newValue = 0;
	int oldMouseY = 0;
	int lastValue = 0;
	bool click = false;
	bool ctrlKey = false;
	bool shiftKey = false;
	bool isNum = false;


	enum PreferredAppMode { Default, AllowDark, ForceDark, ForceLight, Max };
	typedef PreferredAppMode(WINAPI* PfnSetPreferredAppMode)(PreferredAppMode);
	typedef void (WINAPI* PfnFlushMenuThemes)();

	int showEnum(HWND hwnd, const std::vector<std::string>& enumMenu) {
		if (enumMenu.empty()) return -1;

		// Подключаем темную тему
		HMODULE hUxtheme = GetModuleHandleA("uxtheme.dll");
		if (hUxtheme) {
			auto SetPreferredAppMode = (PfnSetPreferredAppMode)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
			auto FlushMenuThemes = (PfnFlushMenuThemes)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(136));
			if (SetPreferredAppMode && FlushMenuThemes) {
				SetPreferredAppMode(AllowDark);
				FlushMenuThemes();
			}
		}

		HMENU hMenu = CreatePopupMenu();
		for (size_t i = 0; i < enumMenu.size(); ++i) {
			AppendMenuA(hMenu, MF_STRING, (UINT_PTR)(i + 1), enumMenu[i].c_str());
		}

		POINT pt;
		GetCursorPos(&pt);

		// ВАЖНО: Устанавливаем фокус на наше окно, чтобы оно ловило Esc
		HWND hOwner = GetAncestor(hwnd, GA_ROOT);
		SetForegroundWindow(hOwner);

		// TPM_RETURNCMD: возвращает ID пункта или 0, если нажали Esc
		UINT flags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_NONOTIFY;
		int selectedId = TrackPopupMenu(hMenu, flags, pt.x, pt.y, 0, hwnd, NULL);

		// После закрытия меню посылаем "пустое" сообщение, чтобы очистить очередь (баг WinAPI)
		PostMessage(hwnd, WM_NULL, 0, 0);

		DestroyMenu(hMenu);
		SetForegroundWindow(vsHWND);

		// Если selectedId == 0, значит нажали Esc или кликнули мимо -> возвращаем -1
		if (selectedId == 0) return -1;

		return selectedId - 1;
	}

	namespace CamUI
	{
		void GrabTimeStamp(int cmdIndex)
		{
			cmdParamDesc[cmdIndex].param[0].value = timer::timeCursor/SAMPLES_IN_FRAME;
		}

		void GrabDirectionAndAngle(int cmdIndex)
		{
			float q = intToFloatDenom;
			auto eye = ViewCam::currentCamera.ViewVec * q + ViewCam::currentCamera.Target * q;
			cmdParamDesc[cmdIndex].param[2].value = (int)XMVectorGetX(eye);
			cmdParamDesc[cmdIndex].param[3].value = (int)XMVectorGetY(eye);
			cmdParamDesc[cmdIndex].param[4].value = (int)XMVectorGetZ(eye);
			auto at = ViewCam::currentCamera.Target * q;
			cmdParamDesc[cmdIndex].param[5].value = (int)XMVectorGetX(at);
			cmdParamDesc[cmdIndex].param[6].value = (int)XMVectorGetY(at);
			cmdParamDesc[cmdIndex].param[7].value = (int)XMVectorGetZ(at);
			auto up = ViewCam::currentCamera.upVec * q;
			cmdParamDesc[cmdIndex].param[8].value = (int)XMVectorGetX(up);
			cmdParamDesc[cmdIndex].param[9].value = (int)XMVectorGetY(up);
			cmdParamDesc[cmdIndex].param[10].value = (int)XMVectorGetZ(up);

			cmdParamDesc[cmdIndex].param[11].value = (int)Camera::viewCam.angle;
		}

		void ProcessCamKeyContextMenu()
		{
			std::vector<std::string> enumMenu = { "grab view camera","grab view camera with timestamp","grab timestamp only" };

			switch (showEnum(hWnd, enumMenu))
			{
			case 0:
				CamUI::GrabDirectionAndAngle(cmdIndex);
				paramEdit::SaveToSource(cmdIndex);
				break;
			case 1:
				CamUI::GrabDirectionAndAngle(cmdIndex);
				CamUI::GrabTimeStamp(cmdIndex);
				paramEdit::SaveToSource(cmdIndex);
				break;

			case 2:
				CamUI::GrabTimeStamp(cmdIndex);
				paramEdit::SaveToSource(cmdIndex);
				break;

			}
		}
	}

	long long getMaxFractionValue(const char* fractionStr) {
		if (!fractionStr) return 0;

		size_t length = std::strlen(fractionStr);
		if (length == 0) return 0;

		// Вычисляем 10^length - 1
		// Например: length=2 -> 100 - 1 = 99
		// length=3 -> 1000 - 1 = 999
		long long maxVal = 1;
		for (size_t i = 0; i < length; ++i) {
			maxVal *= 10;
		}

		return maxVal - 1;
	}

	std::string updateFractionalPart(const char* paramStr, int delta) {
		if (!paramStr || paramStr[0] == '\0') return "";

		std::string original(paramStr);
		size_t length = original.length();

		// 1. Вычисляем верхний предел (насыщение)
		// Для 3 символов это 1000 - 1 = 999
		long long limit = 1;
		for (size_t i = 0; i < length; ++i) limit *= 10;
		long long max_val = limit - 1;

		// 2. Преобразуем текущую строку в число
		long long value = std::stoll(original);

		// 3. Применяем дельту с насыщением
		value += delta;

		if (value > max_val) value = max_val; // Упор в верхнюю границу (99...9)
		if (value < 0) value = 0;             // Упор в нижнюю границу (00...0)

		// 4. Форматируем обратно в строку
		std::string result = std::to_string(value);

		// 5. Дополняем ведущими нулями до исходной длины
		if (result.length() < length) {
			result.insert(0, length - result.length(), '0');
		}

		return result;
	}

	std::string startTextValue;
	bool mPressed = false;
	bool timeAlwaysOn = true;

	//namespace ConstBuf {
		//void LoadObj(const char* name);
	//}

	void OpenLegacyDialog() {

		OPENFILENAME ofn;       // Common dialog box structure
		TCHAR szFile[260] = { 0 }; // Buffer for file name

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "Wavefront OBJ (*.obj)\0*.obj\0All Files (*.*)\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		// Display the Open dialog box
		if (GetOpenFileName(&ofn) == TRUE) {
			
			//ConstBuf::LoadObj(ofn.lpstrFile);

			//std::wcout << L"Selected file: " << ofn.lpstrFile << std::endl;
		}
	}

	float step = 0;

	void Process()
	{
		ui::mousePos = ui::GetCusorPos();
		paramsAreLoaded = true;

		POINT pt;
		GetCursorPos(&pt);

		bool ctrl = GetAsyncKeyState(VK_CONTROL);
		bool shift = GetAsyncKeyState(VK_SHIFT);

		if (GetAsyncKeyState(VK_MBUTTON))
		{
			if (!click && WindowFromPoint(pt) == vsHWND)
			{
				if (VsEditor.Update(false))
				{
					switch (VsEditor.typeUnderCursor)
					{
						case VsEditor.textType::number:
						{
							click = true;

							oldValue = atoi(VsEditor.paramStr);
							startTextValue = std::string(VsEditor.paramStr);
							oldMouseY = pt.y;
							lastValue = oldValue;
							VsEditor.slider = oldValue;

							if (!calcCmdAndParamIndicies() && !fileIsShader()) click = false;

							if (cmdParamDesc[cmdIndex].param[pIndex].bypass) click = false;

							break;
						}

						case VsEditor.textType::enumList:
						{
							if (!calcCmdAndParamIndicies() || mPressed) break;

							mPressed = true;
							int tID = getTypeIndex(VsEditor.typeName);
							int tCnt = getEnumCount(tID);
							std::vector<std::string> enumMenu;
							for (int i = 0; i < tCnt; i++)
							{
								enumMenu.push_back(getStrValue(tID, i));
							}

							int sel = cmdParamDesc[cmdIndex].param[pIndex].value;
							sel = tCnt == 2 ? 1 - sel : showEnum(hWnd, enumMenu);

							if (sel >= 0)
							{
								VsEditor.ReplaceTextInVS(enumMenu[sel].c_str(), true);
								cmdParamDesc[cmdIndex].param[pIndex].value = sel;
							}

							break;
						}

						case VsEditor.textType::functionCall:
						{
							calcCmdAndParamIndicies();
							if (!strcmp(VsEditor.funcName, "setCamKey"))
							{
								CamUI::ProcessCamKeyContextMenu();
							}
							
							break;
						}
					}
				}
			}

			if (click)
			{
				int scale = 1;
				
				if (ctrl != ctrlKey) {
					oldValue = newValue;
					oldMouseY = pt.y;
					ctrlKey = ctrl;
				}

				if (shift != shiftKey) {
					oldValue = newValue;
					oldMouseY = pt.y;
					shiftKey = shift;
				}

				if (ctrl) scale *= 100;
				if (shift) scale *= 10;
				
				int delta = -(pt.y - oldMouseY) * scale/2;

				newValue = oldValue + delta;
				std::string newValueStr;

				if (newValue != lastValue) {

					VsEditor.slider = newValue;

					if (fileIsShader())
					{
						VsEditor.Update(true);

						if (VsEditor.pointBefore)
						{
							newValueStr = updateFractionalPart(startTextValue.c_str(), delta);
							VsEditor.ReplaceTextInVS(newValueStr.c_str(), true);
						}
						else
						{
							char modified[100];
							_itoa(newValue, modified, 10);
							VsEditor.ReplaceTextInVS(modified, true);
						}

						VsEditor.saveChanges();
						recompileShader();
					}
					else
					{
						char modified[100];
						_itoa(newValue, modified, 10);

						if (cmdIndex >= 0 && pIndex >= 0)
						{
							cmdParamDesc[cmdIndex].param[pIndex].value = newValue;
							strcpy(cmdParamDesc[cmdIndex].param[pIndex].strValue, modified);
						}

						VsEditor.Update(true);
						VsEditor.ReplaceTextInVS(modified, true);
					}

					lastValue = newValue;
				}
			}
		}
		else
		{
			click = false;
			mPressed = false;

			if (VsEditor.saveChanges())
			{
				if (fileIsShader())
				{
					recompileShader();
				}
		
				if (VsEditor.Update(false))
				{
					if (calcCmdAndParamIndicies())
					{
						if (VsEditor.typeUnderCursor == VsEditor.textType::number)
						{
							cmdParamDesc[cmdIndex].param[pIndex].value = atoi(VsEditor.paramStr);
							strcpy(cmdParamDesc[cmdIndex].param[pIndex].strValue, VsEditor.paramStr);
						}
						if (VsEditor.typeUnderCursor == VsEditor.textType::text)
						{
							strcpy(cmdParamDesc[cmdIndex].param[pIndex].strValue, VsEditor.textValue);
						}

					}
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

		if (isKeyDown(TIME_KEY)|| timeAlwaysOn)
		{
			TimeLine::ProcessInput();
			TimeLine::Draw();
			paramEdit::showTrack();
		}

		//game mode

		if (!gameCam)
		{
			if (!GetAsyncKeyState(VK_CONTROL))
			{
				if (GetAsyncKeyState('W')) ViewCam::stepAmp += .01;
				if (GetAsyncKeyState('S')) ViewCam::stepAmp -= .01;
				if (GetAsyncKeyState('A')) ViewCam::slideAmp += .01;
				if (GetAsyncKeyState('D')) ViewCam::slideAmp -= .01;


				if (!(GetAsyncKeyState('W') || GetAsyncKeyState('S'))) ViewCam::stepAmp *= .75;
				if (!(GetAsyncKeyState('A') || GetAsyncKeyState('D'))) ViewCam::slideAmp *= .75;
			}

			ViewCam::Dolly();
			ViewCam::Slide(); 
		}
		//

		paramEdit::CamKeys();

		ViewCam::setup();
		ViewCam::setCamMat();

		if (isKeyDown(CAM_KEY)|| ViewCam::flyToCam < 1.f)
		{
			ViewCam::Draw();
		} 
		
		if (paramEdit::ButtonPressed("Load Model", 0, 0, ui::style::box::width, ui::style::box::height))
		{
			loadFlag = true;
		}
		else
		{
			loadFlag = false;
		}

		Rasterizer::Scissors({ 0,0,(float)dx11::width,(float)dx11::height });
		ui::Box::Setup();
		ui::style::Base();
		ui::style::button::hAlign = ui::style::align_h::center;
		ui::style::button::zoom = true;
	}

	void SaveAndExit()
	{
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
	}

}