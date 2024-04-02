#include "..\fx\settings.h"

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <windows.h>

#include <libloaderapi.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include <fstream>
#include <string>
using namespace std;

string shaderFile = "..\\fx\\generated\\processedShaders.h";//final output

string inVPath = "..\\fx\\projectFiles\\shaders\\vs\\";
string inPPath = "..\\fx\\projectFiles\\shaders\\ps\\";
string outVPath = "..\\fx\\generated\\vs\\";
string outPPath = "..\\fx\\generated\\ps\\";

void Log(const char* message)
{
	printf("%s", message);
}

char pathToExe[MAX_PATH];

void SelfLocate()
{
	GetModuleFileNameA(NULL, (LPSTR)pathToExe, MAX_PATH);
	PathRemoveFileSpec((LPSTR)pathToExe);
	SetCurrentDirectory((LPSTR)pathToExe);
}

#define EditMode true //define EditMode for true branch selection in main poject
#include "..\fx\shaders.h"

void Process(string shaderName, string inPath, string outPath, ofstream &ofile)
{
	Log(shaderName.c_str());
	Log("\n");

	ofile << "const char* " << shaderName.c_str() << " = \"";

#ifdef USE_SHADER_MINIFIER
	string _pathToExe = pathToExe;

	string minifierCmdLine = _pathToExe + "\\shader_minifier.exe "+ inPath + shaderName + ".hlsl" + " -o " + outPath + shaderName + ".hlsl " + 
	"--hlsl --format text --no-remove-unused --preserve-all-globals --no-inlining --preserve-externals --preprocess";

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	PROCESS_INFORMATION pi;

	if (CreateProcess(NULL, const_cast<char*>(minifierCmdLine.c_str()), NULL, NULL, FALSE, NULL, NULL, pathToExe, &si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	};

	string inFilePath = outPath + shaderName + ".hlsl";

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

	string inFilePath = inPath + shaderName + ".hlsl";

	ofile << "\\\n";

	string s;
	ifstream ifile(inFilePath);

	while (getline(ifile, s))
	{
		ofile << s << "\\" << endl;
	}

	ofile << "\";\n\n";

#endif

}

int main()
{

	Log("\n---Collecting used shaders and create shader file for runtime\n\n");

	int vShadersCount = sizeof(shaders::vsList) / sizeof(const char*);
	int pShadersCount = sizeof(shaders::psList) / sizeof(const char*);

	SelfLocate();
	remove(shaderFile.c_str());

	ofstream ofile (shaderFile);

	ofile << "//automatically generated file: all used shaders as const char* strings\n\n";
	ofile << "namespace shadersData {\n\n";

	int i = 0;
	while (i < vShadersCount)
	{
		Process(shaders::vsList[i], inVPath, outVPath, ofile); i++;
	}

	i = 0;
	while (i < pShadersCount)
	{
		Process(shaders::psList[i], inPPath, outPPath, ofile); i++;
	}


	ofile << "\n";

	//-- compiler function

	ofile << "void CompileAll ()\n" << "{\n";

	i = 0;
	while (i < vShadersCount)
	{
		ofile << "dx::Shaders::Compiler::Vertex (" <<  i  <<  ", " << shaders::vsList[i] <<  ");\n";	i++;
	}

	i = 0;
	while (i < pShadersCount)
	{
		ofile << "dx::Shaders::Compiler::Pixel (" << i << ", " << shaders::psList[i] << ");\n";	i++;
	}


	ofile << "};\n\n\n};";
	
	ofile.close();
    
	Log("\n---competed!\n\n");

}