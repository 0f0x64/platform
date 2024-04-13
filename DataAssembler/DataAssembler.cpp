#include "..\fx\settings.h"

#define _CRT_SECURE_NO_WARNINGS

#include <cassert>
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
string shaderCompilerFile = "..\\fx\\generated\\processedShadersCompile.h";//final output
string constReflectFile = "..\\fx\\generated\\constBufReflect.h";//final output

string inVPath = "..\\fx\\projectFiles\\shaders\\vs\\";
string inPPath = "..\\fx\\projectFiles\\shaders\\ps\\";
string inLibPath = "..\\fx\\projectFiles\\shaders\\lib\\";
string outVPath = "..\\fx\\generated\\vs\\";
string outPPath = "..\\fx\\generated\\ps\\";
string outLibPath = "..\\fx\\generated\\lib\\";

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

	ofile << "const char* " << shaderName.c_str() << " = ";

#if USE_SHADER_MINIFIER
	string _pathToExe = pathToExe;

	string minifierCmdLine = _pathToExe + "\\shader_minifier.exe " +
		" --hlsl --format text --no-remove-unused --preserve-all-globals --no-inlining --preserve-externals " +
		inPath + shaderName + shaderExtension +
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

	string s;
	ifstream ifile(inFilePath);

	if (ifile.is_open())
	{
		while (getline(ifile, s))
		{
			ofile << '"' << s << "\\n" << '"' << endl;

		}
	}

	ofile << ";\n\n";

#else

	string inFilePath = inPath + shaderName + shaderExtension;

	ofile << "\n";

	string s;
	ifstream ifile(inFilePath);

	while (getline(ifile, s))
	{
		ofile << '"' << s << "\\n" << '"' << endl;
		
	}

	ofile << ";\n\n";

#endif

}

enum sType { vertex, pixel };

/*
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



}*/


void ConstBufReflector(string shaderName, string inPath, ofstream& ofile, sType type, int sIndex)
{
	ofile << "namespace ";
	if (type == sType::vertex) ofile << "vs{\n\n";
	if (type == sType::pixel) ofile << "ps{\n\n";

	ofile << "struct { \n\n";
	
	string inFilePath = inPath + shaderName + shaderExtension;
	ifstream in(inFilePath);

	string tt[2];
	string smp = "SamplerState ";
	string cb = "cbuffer ";
	string cbName = "params";
	string cType = "float ";

	string params = "struct \n{\n";
	string textures = "struct \n{\n";
	string samplers = "struct \n{\n";

	string texturesSet = "";
	int texturesCounter = 0;
	string samplersSet = "";
	int samplersCounter = 0;

	#define found res != string::npos
	#define notFound res == string::npos

	bool isParams = false;
	bool isSamplers = false;
	bool isTextures = false;

	if (in.is_open())
	{
		string s;
		while (getline(in, s))
		{
			tt[0] = "Texture2D ";
			tt[1] = "TextureCube ";

			unsigned int res;

			for (int t = 0; t < 2;t++)
			{
				res = s.find(tt[t]);
				if (found)
				{
					isTextures = true;
					string name = "";
					auto nameStart = s.find_first_not_of(" ", tt[t].length());
					unsigned int i = nameStart;

					while (i < s.length())
					{
						if (s.at(i) == ' ' || s.at(i) == ':') break;
						name.push_back(s.at(i));
						i++;
					}

					textures += "int " + name + ";\n";

					texturesSet += "Textures::SetTexture(textures." + name + ", " + to_string(texturesCounter) + ", ";
					if (type == sType::vertex) texturesSet += "Textures::tAssignType::vertex";
					if (type == sType::pixel) texturesSet += "Textures::tAssignType::pixel";
					texturesSet += "); \n";
					texturesCounter++;

				}
			}

			res = s.find(smp);
			if (found)
			{
				isSamplers = true;
				string name = "";
				auto nameStart = s.find_first_not_of(" ", smp.length());
				unsigned int i = nameStart;
				while (i < s.length())
				{
					if (s.at(i) == ' ' || s.at(i) == ':') break;
					name.push_back(s.at(i));
					i++;
				}

				samplers += "int " + name + "Filter;\n" + "int " + name + "AddressU;\n" + "int " + name + "AddressV;\n";

				if (type == sType::vertex) samplersSet += "Sampler::Set(Sampler::to::vertex, ";
				if (type == sType::pixel) samplersSet += "Sampler::Set(Sampler::to::pixel, ";
				samplersSet += to_string(samplersCounter) + ", " + "samplers." + name + "Filter, " + "samplers." + name + "AddressU, " + "samplers." + name + "AddressV"+ "); \n";
				samplersCounter++;
			}

			res = s.find(cb);
			if (found)
			{
				res = s.find(cbName, res);
				if (found)
				{
					isParams = true;
					unsigned int pos = 0;
					while (true)
					{
						res = s.find("{");
						if (found)
						{
							pos = res;
							break;
						}

						pos = 0;
						assert(getline(in, s));
					}

					while (true)
					{
						unsigned int endCB = s.length();
						auto res = s.find("}", pos);
						if (found) endCB = res;
						if (pos >= endCB) break;

						res = s.find(cType, pos);
						if (found)
						{
							pos = res + cType.length();
							bool done = false;

							while (!done)
							{
								string name = "";
								auto nameStart = s.find_first_not_of(' ', pos);
								unsigned int i = nameStart;
								while (i < endCB)
								{
									if (s.at(i) == ' ')
									{
										while (i < endCB)
										{
											if (s.at(i) != ' ') break;
											i++;
										}
									}

									if (s.at(i) == ';')
									{
										pos = i;
										done = true;
										break;
									}
									if (s.at(i) == ',')
									{
										pos = i + 1;
										break;
									}
									name.push_back(s.at(i));
									i++;
								}
								params.append("float ");
								params.append(name);
								params.append(";\n");
							}
						}
						else
						{
							assert(getline(in, s));
							pos = 0;
						}
					}
				}
			}
		}
	}

	if (isParams) ofile << params << "} params;\n\n";
	if (isTextures) ofile << textures << "} textures;\n\n";
	if (isSamplers) ofile << samplers << "} samplers;\n\n";

	ofile << "void set () {\n";
	if (type == sType::vertex) ofile << "Shaders::SetVS(";
	if (type == sType::pixel) ofile << "Shaders::SetPS(";
	ofile << sIndex << ");\n";
	if (type == sType::vertex && isParams)
	{
		ofile << "memcpy((char*)ConstBuf::drawerV,&params,sizeof(params));\n";
	}
	if (type == sType::pixel && isParams)
	{
		ofile << "memcpy((char*)ConstBuf::drawerP,&params,sizeof(params));\n";
	}

	if (isTextures)	ofile << texturesSet;
	if (isSamplers) ofile << samplersSet;


	ofile << "}\n";

	ofile << "\n} " << shaderName << ";\n\n";
	ofile << "}\n\n";

}

int vShadersCount = 0;
int pShadersCount = 0;
int libShadersCount = 0;
int libShadersCount2 = 0;

void catToFile(const std::filesystem::path& sandbox, ofstream& ofile, std::vector <std::string>& outputName, int& counter, const char* preStr, const char* postStr)
{
	for (auto const& cat : std::filesystem::directory_iterator{ sandbox })
	{
		std::string fName = cat.path().string();
		auto o = fName.rfind("\\", fName.length());
		fName.erase(0, o + 1);
		o = fName.find(shaderExtension);
		fName.erase(o, o + strlen(shaderExtension));

		ofile << preStr << fName.c_str() << postStr;
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
	catToFile(vsSandbox, vsfile, vsList, vShadersCount, "Shader(", ")\n");
	vsfile.close();

	remove(psListFile.c_str());
	ofstream psfile(psListFile);
	catToFile(psSandbox, psfile, psList, pShadersCount, "Shader(", ")\n");
	psfile.close();

	remove(libListFile.c_str());
	ofstream libfile(libListFile);
	libfile << "const char* libName [] = {";
	catToFile(libSandbox, libfile, libList, libShadersCount, "\"", "\", ");
	libfile << "};\n";

	libfile << "const char* libPtr [] = {";
	catToFile(libSandbox, libfile, libList, libShadersCount2, "shadersData::", ", ");
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

	i = 0;
	while (i < vShadersCount)
	{
		Process(vsList[i], inVPath, outVPath, ofile); 
		ConstBufReflector(vsList[i], inVPath, oReflect, sType::vertex, i);
		i++;
	}

	i = 0;
	while (i < pShadersCount)
	{
		Process(psList[i], inPPath, outPPath, ofile); 
		ConstBufReflector(psList[i], inPPath, oReflect,sType::pixel, i);
		i++;
	}

	i = 0;
	while (i < libShadersCount)
	{
		Process(libList[i], inLibPath, outLibPath, ofile);
		i++;
	}

	ofile << "\n\n};";

	ofile.close();

	//-- compiler function

	remove(shaderCompilerFile.c_str());
	ofstream oSCfile(shaderCompilerFile);

	oSCfile << "void Init ()\n" << "{\n\n";

	i = 0;
	while (i < vShadersCount)
	{
		oSCfile << "CreateVS (" <<  i  <<  ", shadersData::" << vsList[i] <<  ");\n";	i++;
	}

	i = 0;
	while (i < pShadersCount)
	{
		oSCfile << "CreatePS (" << i << ", shadersData::" << psList[i] << ");\n";	i++;
	}

	oSCfile << "\n};";

	oSCfile.close();

	oReflect.close();
    
	Log("\n---competed!\n\n");

}