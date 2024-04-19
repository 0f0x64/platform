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

	#include "editorWindow.h"
	#include "hotReload.h"
	#include "uiDraw.h"
	#include "timeLine.h"

	void Init()
	{
		SelfLocate();
		SetRenderWindowPosition();
	}

	void Process()
	{
		api.rt(tex::mainRT);
		api.cull(cullmode::off);
		api.depth(depthmode::off);

		TimeLine::Draw();
	}
}