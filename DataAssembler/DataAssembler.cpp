#define EditMode true

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
#include <regex>

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
		" --hlsl --format Text --no-remove-unused --preserve-all-globals --no-inlining --preserve-externals " +
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

					textures += "texture " + name + ";\n";

					texturesSet += "Textures::TextureToShader((texture)textures." + name + ", " + to_string(texturesCounter) + ", ";
					if (type == sType::vertex) texturesSet += "targetshader::vertex";
					if (type == sType::pixel) texturesSet += "targetshader::pixel";
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

				samplers += "filter " + name + "Filter;\n" + "addr " + name + "AddressU;\n" + "addr " + name + "AddressV;\n";

				if (type == sType::vertex) samplersSet += "Sampler::Sampler(targetshader::vertex, ";
				if (type == sType::pixel) samplersSet += "Sampler::Sampler(targetshader::pixel, ";
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

						if (!getline(in, s)) 
						{
							Log(shaderName.c_str());
							Log(" unexpected eof: unclosed bracket in constant buffer\n");
							ExitProcess(-1);
						};
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
							if (!getline(in, s))
							{
								Log(shaderName.c_str());
								Log(" unexpected eof: <float> type not found in constant buffer\n");
								ExitProcess(-1);
							};

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
	if (type == sType::vertex) ofile << "Shaders::vShader(";
	if (type == sType::pixel) ofile << "Shaders::pShader(";
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


#include "..\fx\types.h"

//TODO:: multiline declaration support
//TODO:: proper namespace parsing
void ScanFile(std::string fname, ofstream& _ofile, std::string marker)
{

	ifstream in(fname);

	if (in.is_open())
	{
		std::vector <std::string> nsName;
		int nsDepth = 0;
		unsigned int res;
		string s;
		while (getline(in, s))
		{

			res = s.find("namespace ");
			if (found)
			{
				std::string ns;
				for (auto x = res + 10; x < s.length(); x++)
				{
					if (s.at(x) == '//') break;
					if (s.at(x) == '{') break;
					if (s.at(x) != ' ') ns.push_back(s.at(x));
				}
				nsName.push_back(ns);
				nsDepth++;
				
			}

			res = s.find(marker);
			if (found)
			{
				bool variadic = false;
				int pCount = 0;
				std::string loader;
				std::string overrider;
					std::string funcParams;
				std::string funcParams_clean;
				std::string funcName;
				std::string pNameList;
				std::string pNameListComplexType;
				std::string pNameListOut;

				res = s.find("(",res+marker.length());
				if (found)
				{
					auto declEnd = s.find(")", res + 1);
					bool noParams = true;
					for (auto x = res + 1; x <= declEnd; x++)
					{
						if (s.at(x) == ',' || s.at(x) == ')') { res = x;  break; }
						if (s.at(x) != ' ') funcName.push_back(s.at(x));
					}

					std::string fReflection = "..\\fx\\generated\\reflection\\" + funcName + "_ref.h";
					ofstream ofile(fReflection);

					for (auto x = res + 1; x < declEnd; x++)
					{
						if (s.at(x) != ' ') noParams = false;
						funcParams.push_back(s.at(x));
					}

					string comma = noParams ? "" : ", ";
					funcParams_clean = "( " + funcParams + ")";
					funcParams = "( CALLER_INFO" + comma + funcParams + ")";


					//decl

					std::string type;
					std::string name;
					unsigned int x = res + 1;

					auto end = s.find(')');
					std::string pCountStr = "";
					while (x < end)
					{
						if (s.at(x) == ' ') {
							x++; continue;
						}

						while (x < end)
						{
							if (s.at(x) == ' ') break;
							type.push_back(s.at(x));
							x++;
						}

						if (type.compare("unsigned") == 0 || type.compare("signed") == 0)
						{
							type.push_back(' ');
							while (x < end)
							{
								if (s.at(x) != ' ') break;
								x++;
							}

							while (x < end)
							{
								if (s.at(x) == ' ') break;
								type.push_back(s.at(x));
								x++;
							}

						}

						while (x < end)
						{
							if (s.at(x) == ' ') {
								x++; continue;
							}

							if (s.at(x) == ',') break;
							if (s.at(x) == '=')
							{
								while (x < end) {
									if (s.at(x) == ',') break;
									x++;
								}
								break;
							}

							name.push_back(s.at(x));
							x++;
						}

						if (name.compare("...") == 0)
						{
							variadic = true;
						}

						if (pCount > 0)
						{
							pNameList.append(", ");
							pNameListOut.append(", ");
							pNameListComplexType.append(", ");
						}

						auto pCountStr = std::to_string(pCount);

						bool isTypeKnown = false;

						if (variadic)
						{
							pNameList = "...";
							pNameListComplexType = "...";
							pNameListOut.clear();
							overrider.clear();
							loader.clear();

							overrider.append("\tcount = cmdParamDesc[cmdCounter].param[0].value[0];\n");
							overrider.append("\tcmdParamDesc[cmdCounter].pCount = count+1;\n");
							overrider.append("\tfor (int i = 1; i <= count; i++)\n\t{\n");
							overrider.append("\t\tva_params [i]= (unsigned char) cmdParamDesc[cmdCounter].param[i].value[0];\n\t}\n");
							loader.append("\tfor (int i = 0; i < 256; i++)\n\t{\n");
							loader.append("\t\tcmdParamDesc[cmdCounter].param[i].bypass = false;\n");
							loader.append("\t\tstrcpy(cmdParamDesc[cmdCounter].param[i].type , \"int\");\n ");
							loader.append("\t\tcmdParamDesc[cmdCounter].param[i].typeIndex = -1;\n ");
							loader.append("\t\tcmdParamDesc[cmdCounter].param[i]._min = 0;\n");
							loader.append("\t\tcmdParamDesc[cmdCounter].param[i]._max = 255;\n");
							loader.append("\t\tcmdParamDesc[cmdCounter].param[i]._dim = 1;\n");
							loader.append("\t}\n");
							

							loader.append("\tfor (int i = 1; i <= count; i++)\n\t{\n");
							loader.append("\t\tcmdParamDesc[cmdCounter].param[i].value[0]= (int)va_params[i];\n\t}\n");

						}
						else
						{
							overrider.append("\tif (!cmdParamDesc[cmdCounter].param[" + pCountStr + "].bypass) ");

							auto typeIndex = getTypeIndex(type.c_str());

							if (isTypeEnum(typeIndex))
							{
								pNameList.append(name);
								pNameListComplexType.append(name);
								pNameListOut.append(name);
								overrider.append(" " + name + " = (" + type + ")cmdParamDesc[cmdCounter].param[" + pCountStr + "].value[0];\n");
								loader.append("\tcmdParamDesc[cmdCounter].param[" + pCountStr + "].value[0] = (int)" + name + ";\n");
								isTypeKnown = true;
							}
							else
							{
								if (getTypeDim(typeIndex) == 3)
								{
									pNameList.append(name + "_x, " + name + "_y, " + name + "_z");
									pNameListComplexType.append(name);
									pNameListOut.append(type + " {" + name + "_x, " + name + "_y, " + name + "_z }");
									overrider.append("\n\t\t{\n\t\t\t" + name + ".x = cmdParamDesc[cmdCounter].param[" + pCountStr + "].value[0];\n");
									overrider.append("\t\t\t" + name + ".y = cmdParamDesc[cmdCounter].param[" + pCountStr + "].value[1];\n");
									overrider.append("\t\t\t" + name + ".z = cmdParamDesc[cmdCounter].param[" + pCountStr + "].value[2];\n");
									overrider.append("\t\t}\n");

									loader.append("\tcmdParamDesc[cmdCounter].param[" + pCountStr + "].value[0] = " + name + ".x;\n");
									loader.append("\tcmdParamDesc[cmdCounter].param[" + pCountStr + "].value[1] = " + name + ".y;\n");
									loader.append("\tcmdParamDesc[cmdCounter].param[" + pCountStr + "].value[2] = " + name + ".z;\n");

									isTypeKnown = true;
								}

								if (getTypeDim(typeIndex) == 4)
								{
									pNameList.append(name + "_x, " + name + "_y, " + name + "_z, " + name + "_w");
									pNameListComplexType.append(name);
									pNameListOut.append(type + " {" + name + "_x, " + name + "_y, " + name + "_z, " + name + "_w }");

									overrider.append("\t" + name + ".x = cmdParamDesc[cmdCounter].param[" + pCountStr + "].value[0];\n");
									overrider.append("\t" + name + ".y = cmdParamDesc[cmdCounter].param[" + pCountStr + "].value[1];\n");
									overrider.append("\t" + name + ".z = cmdParamDesc[cmdCounter].param[" + pCountStr + "].value[2];\n");
									overrider.append("\t" + name + ".w = cmdParamDesc[cmdCounter].param[" + pCountStr + "].value[3];\n");

									loader.append("\tcmdParamDesc[cmdCounter].param[" + pCountStr + "].value[0] = " + name + ".x;\n");
									loader.append("\tcmdParamDesc[cmdCounter].param[" + pCountStr + "].value[1] = " + name + ".y;\n");
									loader.append("\tcmdParamDesc[cmdCounter].param[" + pCountStr + "].value[2] = " + name + ".z;\n");
									loader.append("\tcmdParamDesc[cmdCounter].param[" + pCountStr + "].value[3] = " + name + ".w;\n");

									isTypeKnown = true;
								}

								if (getTypeDim(typeIndex) == 1)
								{
									pNameList.append(name);
									pNameListComplexType.append(name);
									pNameListOut.append(name);
									overrider.append("\t" + name + " = (" + type + ")cmdParamDesc[cmdCounter].param[" + pCountStr + "].value[0];\n");
									loader.append("\tcmdParamDesc[cmdCounter].param[" + pCountStr + "].value[0] = " + name + ";\n");
									isTypeKnown = true;
								}
							}

							if (!isTypeKnown)
							{
								pNameList.append(name);
								pNameListComplexType.append(name);
								pNameListOut.append(name);
								overrider.append("\t" + name + " = (" + type + ")cmdParamDesc[cmdCounter].param[" + pCountStr + "].value[0];\n");
								loader.append("\tcmdParamDesc[cmdCounter].param[" + pCountStr + "].value[0] = " + name + ";\n");
							}
							else
							{
								loader.append("\tcmdParamDesc[cmdCounter].param[" + pCountStr + "].bypass = false;\n");
							}

							loader.append("\tstrcpy(cmdParamDesc[cmdCounter].param[" + pCountStr + "].type, \"" + type + "\"); \n");
							loader.append("\tstrcpy(cmdParamDesc[cmdCounter].param[" + pCountStr + "].name, \"" + name + "\"); \n");
						}

						name.clear();
						type.clear();
						pCount++;
						x++;

					}

					if (marker == "API")
					{
						for (int nsi = 0; nsi < nsDepth; nsi++)
						{
							_ofile << "namespace " + nsName[nsi] + " {\n";
						}

						_ofile << "#define " << funcName << "(";
						_ofile << pNameList << ") " << funcName << "_ref (__FILE__, __LINE__";
						if (pCount > 0) _ofile << ", ";
						if (variadic) _ofile << "__VA_ARGS__";
						_ofile << pNameListOut << ")\n";

						if (pCount > 0 && !variadic)
						{
							_ofile << "enum class " << funcName << "_param {" << pNameListComplexType << "};\n";
							
						}

						_ofile << "void " << funcName << "_impl" << funcParams_clean << ";\n";
						_ofile << "void " << funcName << "_ref " << funcParams << "\n";
						_ofile << "{\n";
						//reflection code
						if (variadic)
						{
							_ofile << "VA_READ\n\n";
						}

						if (pCount > 0)
						{
							_ofile << "if (paramsAreLoaded) {\n";
							_ofile << overrider;
							_ofile << "} else {\n";

							_ofile << "\tstrcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);\n";
							_ofile << "\tcmdParamDesc[cmdCounter].caller.line = srcLine;\n";
							auto st = variadic ? "count+1" : std::to_string(pCount);
							if (variadic)
							{
								_ofile << "\tcmdParamDesc[cmdCounter].param[0].value[0] = count ;\n";
							}
							_ofile << "\tcmdParamDesc[cmdCounter].pCount = " << st << ";\n";

							if (!variadic)
							{
								loader.append("\n\t\teditor::paramEdit::setParamsAttr();\n");
								loader.append("\t\teditor::paramEdit::setBypass();\n");
							}

							_ofile << loader;
							_ofile << "}\n";
						}

						_ofile << "\nAddToUI(\""<< funcName <<"\"); \n";
						_ofile << "cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;\n";

						_ofile << "cmdLevel++;\n\n";
						_ofile << "cmdCounter++;\n\n";
						
						//caller
						_ofile << funcName << "_impl (";
						if (variadic)
						{
							_ofile << "count, 0 ";
						}
						else
						{
							_ofile << pNameListComplexType;
						}
						_ofile << ");\n";

						//post-call - adjust cmd level;
						
						_ofile << "cmdLevel--;\n";
						_ofile << "}\n\n";

						for (int nsi = 0; nsi < nsDepth; nsi++)
						{
							_ofile << "}\n";
						}

					}
				}

			}
		}
	}

}

void srcCat(const std::filesystem::path& sandbox, ofstream& ofile)
{
	for (auto const& cat : std::filesystem::recursive_directory_iterator{ sandbox })
	{
		std::string fName = cat.path().string();
		auto o = fName.rfind(".", fName.length());
		if (o != string::npos)
		{
			if (o == fName.length()-2)
			{
				if (fName.at(o + 1) = 'h')
				{
						ScanFile(fName, ofile, "API");
				}
			}
		}

	}
}

void deleteDirectoryContents(const std::filesystem::path& dir)
{
	for (const auto& entry : std::filesystem::directory_iterator(dir))
		std::filesystem::remove_all(entry.path());
}

int main()
{
	SelfLocate();

	const std::filesystem::path vsSandbox{ inVPath };
	const std::filesystem::path psSandbox{ inPPath };
	const std::filesystem::path libSandbox{ inLibPath };

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


	//-- collect al marked functions and create reflection structure
	Log("\nReflected functions\n");

	std::string apiFileName = "..\\fx\\generated\\apiReflection.h";
	remove(apiFileName.c_str());
	ofstream apiOfile(apiFileName);

	std::filesystem::path fref{ "..\\fx\\generated\\reflection\\" };
	deleteDirectoryContents(fref);

	const std::filesystem::path srcSandbox{ "..\\fx\\projectFiles\\" };
	srcCat(srcSandbox, apiOfile);
	const std::filesystem::path srcSandbox2{ "..\\fx\\dx11\\" };
	srcCat(srcSandbox2, apiOfile);


	apiOfile.close();

	//--

	Log("\n---competed!\n\n");

}