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

bool minifier = false;
bool final_release = false;

string uiShaderMargin = "..\\fx\\generated\\uiShaderMargin.h";//final output

string shaderFile = "..\\fx\\generated\\processedShaders.h";//final output
string shaderCompilerFile = "..\\fx\\generated\\processedShadersCompile.h";//final output
string constReflectFile = "..\\fx\\generated\\constBufReflect.h";//final output

string UIinVPath = "..\\fx\\projectFiles\\shaders\\ui\\vs\\";
string UIinPPath = "..\\fx\\projectFiles\\shaders\\ui\\ps\\";

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
	OutputDebugString(message);
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

void Process(string shaderName, string inPath, string outPath, ofstream& ofile)
{
	Log(shaderName.c_str());

	ofile << "const char* " << shaderName.c_str() << " = ";

	if (minifier)
	{
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

		Log(" - compressed by minifier");
	}
	else
	{
		string inFilePath = inPath + shaderName + shaderExtension;

		ofile << "\n";

		string s;
		ifstream ifile(inFilePath);

		while (getline(ifile, s))
		{
			ofile << '"' << s << "\\n" << '"' << endl;

		}

		ofile << ";\n\n";

	}

	Log("\n");

}

void removeDoubleSpaces(std::string& str) {
	std::regex pattern("\\s{2,}");
	str = std::regex_replace(str, pattern, " ");
}

std::vector<std::string> regex_split(const std::string& str, const std::regex& reg) {

	const std::sregex_token_iterator beg{ str.cbegin(), str.cend(), reg, -1 };
	const std::sregex_token_iterator end{};
	return { beg, end };
}

enum sType { vertex, pixel };

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

			for (int t = 0; t < 2; t++)
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
				samplersSet += to_string(samplersCounter) + ", " + "samplers." + name + "Filter, " + "samplers." + name + "AddressU, " + "samplers." + name + "AddressV" + "); \n";
				samplersCounter++;
			}

			std:string cbText;

			res = s.find(cb);
			if (found)
			{
				res = s.find(cbName, res);
				if (found)
				{
					isParams = true;

					res = s.find("{");
					if (found)
					{
						auto lbr = res;
						res = s.find("}");
						if (found)
						{
							cbText.append(s.substr(lbr + 1, res - (lbr + 1)));
							params += cbText;
							break;
						}
						else
						{
							cbText.append(s.substr(lbr + 1, s.length() - (lbr + 1)));
						}
					}
					else
					{

						bool lbr = false;

						while (true)
						{
							char a;
							in.get(a);
							if (a == '{')
							{
								break;
							}
						}
					}
					
					while (true)
					{
						char a;
						in.get(a);
						if (a == '}')
						{
							break;
						}

						cbText += a;
					}



					removeDoubleSpaces(cbText);
					std::erase(cbText, '\t');
					std::erase(cbText, '\n');
					if (cbText.at(0) == ' ') cbText.erase(0, 1);
					while (cbText.at(cbText.length() - 1) == ' ') cbText.erase(cbText.length() - 1, 1);

					const std::regex reg{ R"(;)" };
					auto tokens = regex_split(cbText, reg);

					for (int i = 0;i < tokens.size();i++)
					{
						while (tokens[i].at(0) == ' ') tokens[i].erase(0, 1);
						

						if (tokens[i].find(",") != std::string::npos)
						{
							tokens[i].at(tokens[i].find(" ")) = ',';

							const std::regex reg{ R"(,)" };
							auto tokens_ins = regex_split(tokens[i], reg);
							for (int j = 1;j < tokens_ins.size();j++)
							{
								std::erase(tokens_ins[j], ' ');
								params += tokens_ins[0]+" "+ tokens_ins[j]+";\n";

							}
						}
						else
						{
							params += tokens[i]+";\n";
						}

					}

					//params += cbText;

					unsigned int pos = 0;
					/*while (true)
					{
						res = s.find("{");
						if (found)
						{
							pos = res;
							break;
						}

						pos = 0;

/*						if (!getline(in, s))
						{
							Log(shaderName.c_str());
							Log(" unexpected eof: unclosed bracket in constant buffer\n");
							ExitProcess(-1);
						};
					}*/



					//while (true)
					{

						/*unsigned int endCB = s.length();
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
						}*/
					}
				}
			}
		}
	}

	if (isParams) { 
		ofile << params << "} params;\n\n"; 
	}
	else {
		ofile << "struct {} params;\n\n";
	}

	if (isTextures) ofile << textures << "} textures;\n\n";
	if (isSamplers) ofile << samplers << "} samplers;\n\n";

	ofile << "void set () {\n";

	if (type == sType::vertex) ofile << "Shaders::vShader(";
	if (type == sType::pixel) ofile << "Shaders::pShader(";
	ofile << sIndex << ");\n";

	if (type == sType::vertex && isParams)
	{
		ofile << "context->UpdateSubresource(dx11::Shaders::VS[" << sIndex << "].params, 0, NULL, &params, 0, 0);\n";
		ofile << "context->VSSetConstantBuffers(0, 1, &dx11::Shaders::VS[" << sIndex << "].params);\n";
	}
	if (type == sType::pixel && isParams)
	{
		ofile << "context->UpdateSubresource(dx11::Shaders::PS[" << sIndex << "].params, 0, NULL, &params, 0, 0);\n";
		ofile << "context->PSSetConstantBuffers(0, 1, &dx11::Shaders::PS[" << sIndex << "].params);\n";
	}

	if (isTextures)	ofile << texturesSet;
	if (isSamplers) ofile << samplersSet;

	ofile << "}\n";

	ofile << "\n} " << shaderName << ";\n\n";
	ofile << "}\n\n";

}

int UIvShadersStart = 0;
int UIpShadersStart = 0;

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

int main(int argc, char* argv[])
{
	for (int i{}; i < argc; ++i)
	{
		if (strcmp(argv[i], "-minifier") == 0)
		{
			minifier = true;
			Log("using shader minifier\n");
		}

		if (strcmp(argv[i], "-release") == 0)
		{
			final_release = true;
			Log("release config - ui shaders will be removed\n");
		}

	}

	SelfLocate();

	const std::filesystem::path UIvsSandbox{ UIinVPath };
	const std::filesystem::path UIpsSandbox{ UIinPPath };

	const std::filesystem::path vsSandbox{ inVPath };
	const std::filesystem::path psSandbox{ inPPath };
	const std::filesystem::path libSandbox{ inLibPath };

	int i = 0;

	Log("\n---scan shader source dir and create macro definitions\n\n");

	remove(vsListFile.c_str());
	ofstream vsfile(vsListFile);
	catToFile(vsSandbox, vsfile, vsList, vShadersCount, "Shader(", ")\n");

	UIvShadersStart = vShadersCount;
	
	if (!final_release)
	{
		catToFile(UIvsSandbox, vsfile, vsList, vShadersCount, "Shader(", ")\n");
	}

	vsfile.close();

	remove(psListFile.c_str());
	ofstream psfile(psListFile);
	catToFile(psSandbox, psfile, psList, pShadersCount, "Shader(", ")\n");

	UIpShadersStart = pShadersCount;

	if (!final_release)
	{
		catToFile(UIpsSandbox, psfile, psList, pShadersCount, "Shader(", ")\n");
	}

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
	ofstream ofile(shaderFile);

	remove(constReflectFile.c_str());
	ofstream oReflect(constReflectFile);

	ofile << "//automatically generated file: all used shaders as const char* strings\n\n";
	ofile << "namespace shadersData {\n\n";

	i = 0;
	while (i < vShadersCount)
	{
		bool UImargin = i >= UIvShadersStart;
		Process(vsList[i], UImargin ? UIinVPath : inVPath, outVPath, ofile);
		ConstBufReflector(vsList[i], UImargin ? UIinVPath : inVPath, oReflect, sType::vertex, i);
		i++;
	}

	i = 0;
	while (i < pShadersCount)
	{
		bool UImargin = i >= UIpShadersStart;
		Process(psList[i], UImargin ? UIinPPath : inPPath, outPPath, ofile);
		ConstBufReflector(psList[i], UImargin ? UIinPPath : inPPath, oReflect, sType::pixel, i);
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

	remove(uiShaderMargin.c_str());
	ofstream uiMfile(uiShaderMargin);

	if (!final_release)
	{
		uiMfile << "int UIvShadersStart = " << UIvShadersStart << ";\n";
		uiMfile << "int UIpShadersStart = " << UIpShadersStart << ";\n";
	}

	uiMfile.close();

	remove(shaderCompilerFile.c_str());
	ofstream oSCfile(shaderCompilerFile);

	oSCfile << "void Init ()\n" << "{\n\n";

	i = 0;
	while (i < vShadersCount)
	{
		oSCfile << "CreateVS (" << i << ", shadersData::" << vsList[i] << ");\n";	i++;
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

	//getchar();

}