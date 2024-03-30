#include <iostream>
#include <windows.h>

#include <libloaderapi.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#define FILEBUFFERSIZE 1024*1024
char* fileBuffer;
char* fileBuffer2;

void Log(const char* message)
{
	OutputDebugString(message);
}

char name[MAX_PATH];

void SelfLocate()
{
	GetModuleFileNameA(NULL, (LPSTR)name, MAX_PATH);
	PathRemoveFileSpec((LPSTR)name);
	SetCurrentDirectory((LPSTR)name);
}

int vsCounter = 0;
int psCounter = 0;

#define shader(VariableName) VariableName
enum shaders {
    #include "..\fx\projectFiles\shaders.h"
};

#undef shader
#define shader(VariableName) # VariableName
const char* shaderNameList[] = {
    #include "..\fx\projectFiles\shaders.h"
};

int main()
{
	Log("Collecting used shaders and create shader file for runtime\n");

	fileBuffer = new char[FILEBUFFERSIZE];
	fileBuffer2 = new char[FILEBUFFERSIZE];

	int len = sizeof(shaderNameList) / sizeof(const char*);

	const char* shaderFile = "generated\shaders.h";
	SelfLocate();
	remove(shaderFile);
	FILE* result = fopen(shaderFile, "w");

	int i = 0;
	while (i<2)
	{
		Log(shaderNameList[i]);
		Log("\n");


		fprintf(result, "%s", shaderNameList[i]);
		fprintf(result, "\n");

		i++;
	}

	fclose(result);

	delete(fileBuffer);
	delete(fileBuffer2);
    
}