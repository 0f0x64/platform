#include "..\fx\settings.h"

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <windows.h>
#include <string>
#include <filesystem>

#include <libloaderapi.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include <fstream>

using namespace std;

string shaderFile = "..\\fx\\generated\\processedShaders.h";//final output
string constReflectFile = "..\\fx\\generated\\constBufReflect.h";//final output

string inVPath = "..\\fx\\projectFiles\\shaders\\vs\\";
string inPPath = "..\\fx\\projectFiles\\shaders\\ps\\";
string inLibPath = "..\\fx\\projectFiles\\shaders\\lib\\";
string outVPath = "..\\fx\\generated\\vs\\";
string outPPath = "..\\fx\\generated\\ps\\";

string vsListFile = "..\\fx\\generated\\vsList.h";
string psListFile = "..\\fx\\generated\\psList.h";
string libListFile = "..\\fx\\generated\\libList.h";

const char* shaderExtension = ".shader";

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

std::vector <std::string> vsList;
std::vector <std::string> psList;
std::vector <std::string> libList;

void Process(string shaderName, string inPath, string outPath, ofstream &ofile)
{
	Log(shaderName.c_str());
	Log("\n");

	ofile << "const char* " << shaderName.c_str() << " = \"";

#if USE_SHADER_MINIFIER
	string _pathToExe = pathToExe;

	string minifierCmdLine = _pathToExe + "\\shader_minifier.exe " + inPath + shaderName + shaderExtension +
		" --hlsl --format text --no-remove-unused --preserve-all-globals --no-inlining --preserve-externals" +
		" -o " + outPath + shaderName + shaderExtension;
	


	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	PROCESS_INFORMATION pi;

	if (CreateProcess(NULL, const_cast<char*>(minifierCmdLine.c_str()), NULL, NULL, FALSE, NULL, NULL, pathToExe, &si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	};

	string inFilePath = outPath + shaderName + shaderExtension;

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

	string inFilePath = inPath + shaderName + shaderExtension;

	//ofile << "\\\n";

	string s;
	ifstream ifile(inFilePath);

	while (getline(ifile, s))
	{
		//ofile << s << "\\" << endl;
		ofile << s ;
	}

	ofile << "\";\n\n";

#endif

}

void ProcessLib(string shaderName, string inPath, string outPath, ofstream& ofile)
{
	Log(shaderName.c_str());
	Log("\n");

	ofile << "const char* " << shaderName.c_str() << " = \"";

	string inFilePath = inPath + shaderName + shaderExtension;

	string s;
	ifstream ifile(inFilePath);

	while (getline(ifile, s))
	{
		ofile << s;
	}
	ofile << "\\n";
	ofile << "\";\n\n";

}


#include <d3dcompiler.h>
#include <d3d11shader.h>
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

ID3DBlob* pBlob;
ID3DBlob* pErrorBlob;

wchar_t shaderPathW[MAX_PATH];

LPCWSTR nameToPatchLPCWSTR(const char* name)
{
	char path[MAX_PATH];
	strcpy(path, name);
	

	int len = MultiByteToWideChar(CP_ACP, 0, path, -1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, path, -1, shaderPathW, len);
	return shaderPathW;
}


void reflect(ofstream& ofile, string &shaderName)
{
	ID3D11ShaderReflection* pReflector = NULL;
	D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector);
	D3D11_SHADER_DESC shaderDesc;
	pReflector->GetDesc(&shaderDesc);

	for (UINT i = 0; i < shaderDesc.ConstantBuffers; i++)
	{
		ID3D11ShaderReflectionConstantBuffer* constBuffReflection = pReflector->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC shaderBuffDesc;
		constBuffReflection->GetDesc(&shaderBuffDesc);

		if (!strcmp(shaderBuffDesc.Name, "params"))
		{
			if (shaderBuffDesc.Variables > 0)
			{
				ofile << "enum " << shaderName << " { ";

				for (UINT j = 0; j < shaderBuffDesc.Variables; j++)
				{
					ID3D11ShaderReflectionVariable* variableRefl = constBuffReflection->GetVariableByIndex(j);
					D3D11_SHADER_VARIABLE_DESC shaderVarDesc;
					variableRefl->GetDesc(&shaderVarDesc);

					ofile << shaderVarDesc.Name;
					ofile << ", ";
				}

				ofile << " };\n";
			}
		}

	}
}

enum sType {vertex,pixel};

void ConstBufReflector(string shaderName, string inPath, ofstream& ofile, sType type)
{
	string inFilePath = inPath + shaderName + shaderExtension;
	LPCWSTR source = nameToPatchLPCWSTR(inFilePath.c_str());

	HRESULT hr;
	if (type == sType::vertex)
	{
		pBlob = NULL;
		hr = D3DCompileFromFile(source, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_4_1", D3DCOMPILE_SKIP_OPTIMIZATION , NULL, &pBlob, &pErrorBlob);
		if (FAILED(hr))
		{
			char* m = (char*)pErrorBlob->GetBufferPointer();
			Log(m);
		}
	}
	if (type == sType::pixel)
	{
		pBlob = NULL;
		hr = D3DCompileFromFile(source, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_4_1", D3DCOMPILE_SKIP_OPTIMIZATION, NULL, &pBlob, &pErrorBlob);
		if (FAILED(hr))
		{
			char* m = (char*)pErrorBlob->GetBufferPointer();
			Log(m);
		}
	}
	

	
	reflect(ofile,shaderName);



}

int vShadersCount = 0;
int pShadersCount = 0;
int libShadersCount = 0;
int libShadersCount2 = 0;

void catToFile(const std::filesystem::path &sandbox, ofstream &ofile, std::vector <std::string> &outputName, int &counter)
{
	for (auto const& cat : std::filesystem::directory_iterator{ sandbox })
	{
		std::string fName = cat.path().string();
		auto o = fName.rfind("\\", fName.length());
		fName.erase(0, o + 1);
		o = fName.find(shaderExtension);
		fName.erase(o, o + 5);

		ofile << "Shader(" << fName.c_str() << ")\n";
		outputName.push_back(fName);
		counter++;
	}
}

void catToFileLib(const std::filesystem::path& sandbox, ofstream& ofile, std::vector <std::string>& outputName, int& counter)
{
	for (auto const& cat : std::filesystem::directory_iterator{ sandbox })
	{
		std::string fName = cat.path().string();
		auto o = fName.rfind("\\", fName.length());
		fName.erase(0, o + 1);
		o = fName.find(shaderExtension);
		fName.erase(o, o + 5);

		ofile << '"' << fName.c_str() << '"' << ", ";
		outputName.push_back(fName);
		counter++;
	}
}

void catToFileLibPtr(const std::filesystem::path& sandbox, ofstream& ofile, std::vector <std::string>& outputName, int& counter)
{
	for (auto const& cat : std::filesystem::directory_iterator{ sandbox })
	{
		std::string fName = cat.path().string();
		auto o = fName.rfind("\\", fName.length());
		fName.erase(0, o + 1);
		o = fName.find(shaderExtension);
		fName.erase(o, o + 5);

		ofile << "shadersData::" << fName.c_str() << ", ";
		outputName.push_back(fName);
		counter++;
	}
}

int main()
{
	SelfLocate();

	const std::filesystem::path vsSandbox{ "..\\fx\\projectFiles\\shaders\\vs\\" };
	const std::filesystem::path psSandbox{ "..\\fx\\projectFiles\\shaders\\ps\\" };
	const std::filesystem::path libSandbox{ "..\\fx\\projectFiles\\shaders\\lib\\" };

	int i = 0;

	Log("\n---scan shader source dir and create macro definitions\n\n");

	remove(vsListFile.c_str());
	ofstream vsfile(vsListFile);
	catToFile(vsSandbox, vsfile, vsList, vShadersCount);
	vsfile.close();

	remove(psListFile.c_str());
	ofstream psfile(psListFile);
	catToFile(psSandbox, psfile, psList, pShadersCount);
	psfile.close();

	remove(libListFile.c_str());
	ofstream libfile(libListFile);
	libfile << "const char* libName [] = {";
	catToFileLib(libSandbox, libfile, libList, libShadersCount);
	libfile << "};\n";

	libfile << "const char* libPtr [] = {";
	catToFileLibPtr(libSandbox, libfile, libList, libShadersCount2);
	libfile << "};\n";

	libfile << "int libCount = " << to_string(libShadersCount) << ";";

	libfile.close();

	Log("\n---Collecting used shaders and create shader file for runtime\n\n");

	remove(shaderFile.c_str());
	ofstream ofile (shaderFile);
	
	remove(constReflectFile.c_str());
	ofstream oReflect(constReflectFile);

	ofile << "//automatically generated file: all used shaders as const char* strings\n\n";
	ofile << "namespace shadersData {\n\n";

	oReflect << "struct params {\n";

	i = 0;
	while (i < vShadersCount)
	{
		Process(vsList[i], inVPath, outVPath, ofile); 
		ConstBufReflector(vsList[i], inVPath, oReflect, sType::vertex);
		i++;
	}

	i = 0;
	while (i < pShadersCount)
	{
		Process(psList[i], inPPath, outPPath, ofile); 
		ConstBufReflector(psList[i], inPPath, oReflect,sType::pixel);
		i++;
	}

	i = 0;
	while (i < libShadersCount)
	{
		ProcessLib(libList[i], inLibPath, outPPath, ofile);
		i++;
	}

	oReflect << "};\n";

	ofile << "\n";

	//-- compiler function

	ofile << "void CompileAll ()\n" << "{\n";

	i = 0;
	while (i < vShadersCount)
	{
		ofile << "Vertex (" <<  i  <<  ", " << vsList[i] <<  ");\n";	i++;
	}

	i = 0;
	while (i < pShadersCount)
	{
		ofile << "Pixel (" << i << ", " << psList[i] << ");\n";	i++;
	}


	ofile << "};\n\n\n};";
	
	ofile.close();
	oReflect.close();
    
	Log("\n---competed!\n\n");

}