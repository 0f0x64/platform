#define USE_SHADER_MINIFIER

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <windows.h>

#include <libloaderapi.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include <shellapi.h>

#include <fstream>
#include <string>
using namespace std;

void Log(const char* message)
{
	//OutputDebugString(message);
	printf("%s", message);
}

char pathToExe[MAX_PATH];

void SelfLocate()
{
	GetModuleFileNameA(NULL, (LPSTR)pathToExe, MAX_PATH);
	PathRemoveFileSpec((LPSTR)pathToExe);
	SetCurrentDirectory((LPSTR)pathToExe);
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

	Log("\n---Collecting used shaders and create shader file for runtime\n\n");

	int shadersCount = sizeof(shaderNameList) / sizeof(const char*);

	const char* shaderFile = "..\\fx\\generated\\shaders.h";
	SelfLocate();
	remove(shaderFile);

	ofstream ofile(shaderFile);

	ofile << "//automatically generated file: all used shaders as const char* strings\n\n";
	ofile << "namespace shadersData {\n\n";

	int i = 0;
	while (i < shadersCount)
	{
		Log(shaderNameList[i]);
		Log("\n");

		ofile << "const char* " << shaderNameList[i] << " = \"";

		char inFilePath[4096];
		strcpy(inFilePath, "..\\fx\\projectFiles\\shaders\\");
		strcat(inFilePath, shaderNameList[i]);
		strcat(inFilePath, ".hlsl");

	#ifdef USE_SHADER_MINIFIER
		char  minifierCmdLine[4096];
		strcpy(minifierCmdLine, pathToExe);
		strcat(minifierCmdLine, "\\shader_minifier.exe ");
		strcat(minifierCmdLine, "..\\fx\\projectFiles\\shaders\\");
		strcat(minifierCmdLine, shaderNameList[i]);
		strcat(minifierCmdLine, ".hlsl");

		strcat(minifierCmdLine, " ");
		strcat(minifierCmdLine, "-o ");

		strcat(minifierCmdLine, "..\\fx\\generated\\");
		strcat(minifierCmdLine, shaderNameList[i]);
		strcat(minifierCmdLine, ".hlsl ");

		strcat(minifierCmdLine,	"--hlsl --format text --no-remove-unused --preserve-all-globals --no-inlining --preserve-externals");

		STARTUPINFO si;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		PROCESS_INFORMATION pi;
		if (CreateProcess(NULL, minifierCmdLine, NULL, NULL, FALSE, NULL, NULL, pathToExe, &si, &pi))
		{
			WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		};

		strcpy(inFilePath, "..\\fx\\generated\\");
		strcat(inFilePath, shaderNameList[i]);
		strcat(inFilePath, ".hlsl");

		char ch;
		ifstream ifile(inFilePath);
		if (ifile.is_open())
		{
			while (ifile.get(ch))
			{
				ofile << ch;
			}
		}

		ofile << "\";\n\n";

	#else
		ofile << "\\\n";

		string s;
		ifstream ifile(inFilePath);

		while (getline(ifile, s))
		{
			ofile << s << "\\" << endl;
		}

		ofile << "\";\n\n";

	#endif



		i++;
	}

	ofile << "\n";

	//-- compiler function

	ofile << "void CompileAll ()\n" << "{\n";

	i = 0;
	while (i < shadersCount)
	{
		ofile << "dx::CompileShader (" <<  i  <<  ", " <<  shaderNameList[i] <<  ");\n";
		i++;
	}

	ofile << "};\n\n" << "\n};";
	
	ofile.close();
    
	Log("\n---competed!\n");
	Log("\n");
}