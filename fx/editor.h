
#include <libloaderapi.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")



namespace editor
{
	LPSTR name[MAX_PATH];

	void SelfLocate()
	{
		
		GetModuleFileName(NULL, (LPSTR)name, MAX_PATH);
		PathRemoveFileSpec((LPSTR)name);
		SetCurrentDirectory((LPSTR)name);
		//	silently_remove_directory(undopath);
		//	CreateDirectory(undopath, NULL);

	}

	void Init()
	{
		SelfLocate();
	}
}