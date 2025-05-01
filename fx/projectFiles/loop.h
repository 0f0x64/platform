
#include "graphicsAPI.h"

#include "camera.h"
#include "cubemap.h"
#include "object.h"

#include "sound\tracker.h"

int precalcOfs = 0;

namespace Loop
{

	bool isPrecalc = false;

	void Precalc()
	{
		gfx::SetInputAsm(topology::lineList);
		ConstBuf::Update(ConstBuf::cBuffer::global);
		for (int i = 0; i < 6; i++) { ConstBuf::ConstToVertex((ConstBuf::cBuffer)i); ConstBuf::ConstToPixel((ConstBuf::cBuffer)i); }
		isPrecalc = true;
		precalcOfs = cmdCounter;
	}

	void frameConst()
	{
		ConstBuf::frame = {
			.time = XMFLOAT4{ (float)(timer::frameBeginTime * .01) ,(float)timer::timeCursor / SAMPLES_IN_FRAME,0,0},
			.aspect = XMFLOAT4{dx11::aspect,dx11::iaspect, 0, 0}
		};

		ConstBuf::Update(ConstBuf::cBuffer::frame);
	}

	API(cameraMan)
	{
		BasicCam::setCamKey(0, keyType::set, 3021, 22, 46, 2, 51, 0, 0, 508, 77, 65, sliderType::follow, 0, 0, -1, camAxis::local, 0, 3, 0, 0);
		BasicCam::processCam();
	}

#define REFLECTION1




#define CALLER ,const std::source_location caller = std::source_location::current()

	void AddToUI2(const char* funcName)
	{
		cmdParamDesc[cmdCounter].stackLevel = cmdLevel;

		//int x = strlen(funcName) - 1;;
		//for (x; x > 0; x--) { if (funcName[x] == '.') { x++; break; } };

		//strcpy(cmdParamDesc[cmdCounter].funcName, funcName + x);
	}

	void removeDoubleSpaces(std::string& str) {
		std::regex pattern("\\s{2,}");
		str = std::regex_replace(str, pattern, " ");
	}

	void getTypeAndName(std::string& src, std::string& typeStr, std::string& nameStr)
	{
		bool type = false;
		bool name = false;

		for (int j = 0; j < src.size(); j++)
		{
			char a = src.at(j);

			if (a != ' ' && typeStr.empty()) type = true;
			if (a == ' ' && !typeStr.empty()) type = false;

			if (a != ' ' && !typeStr.empty() && !type) name = true;
			if (a == ' ' && !typeStr.empty() && !nameStr.empty()) break;

			if (type) typeStr += a;
			if (name) nameStr += a;

		}
	}


	void SetParamType(int index, const char* type)
	{
		strcpy(cmdParamDesc[cmdCounter].param[index].type, type);
		cmdParamDesc[cmdCounter].param[index].typeIndex = getTypeIndex(type);
		auto base_sz = sizeof(int);
		cmdParamDesc[cmdCounter].param[index].size = base_sz * cmdParamDesc[cmdCounter].param[index]._dim;
	}

	void SetParamName(int index, const char* name)
	{
		strcpy(cmdParamDesc[cmdCounter].param[index].name, name);
	}

	void FillCaller(const std::source_location caller)
	{
		strcpy(cmdParamDesc[cmdCounter].caller.fileName, caller.file_name());
		cmdParamDesc[cmdCounter].caller.line = caller.line();
		cmdParamDesc[cmdCounter].caller.column = caller.column();
	}

	void CopyStrWithSkipper(std::string& in, std::string& lineStr, std::string skippers )
	{
		unsigned int t = 0;
		while (t < in.length())
		{
			bool passed = true;
			for (int i = 0;i < skippers.size();i++)
			{
				if (in.at(t) == skippers.at(i)) passed = false;
			}

			if (passed)	lineStr += in.at(t);
			

			t++;
		}
	}

	bool isParam(std::string line)
	{
		char* p;
		strtol(line.c_str(), &p, 10);
		return *p == 0;
	}

	void SaveToSource()
	{

	}

	void reflect_f(auto* in, const std::source_location caller, const std::source_location currentFunc)//name and types without names
	{
		auto c = &cmdParamDesc[cmdCounter];
		c->reflection_type = 1;

		if (!paramsAreLoaded)  //variables -> reflected struct
		{
			FillCaller(caller);

			std::string fn = currentFunc.function_name();
			auto rb = fn.find("(");
			auto fp = fn.rfind("::", rb);
			auto op = fn.rfind("::", fp - 2);
			std::string objName = fn.substr(op+2, fp - op - 2);
			std::string funcName;
			std::ifstream ifile(currentFunc.file_name());
			std::string s;
			
			int lc = 1;

			bool obj_is_found = false;

			if (ifile.is_open())
			{
				while (true)
				{
					if (!getline(ifile, s)) break;
					if (lc == currentFunc.line()) break;

					std::string lineStr;
					CopyStrWithSkipper(s, lineStr, " \t");

					if (lineStr == "struct" + objName + "{")
					{
						obj_is_found = true;
					}

					if (obj_is_found)
					{

						if (lineStr == "structparams{")
						{
							std::string pStr;
							while (true)
							{
								char a;
								ifile.get(a);
								if (a == '}') break;
								if (a != '\t' && a!= '\n') pStr += a;
							}

							const std::regex reg{ R"(;)" };
							auto tokens = regex_split(pStr, reg);
							
							int param_ofs = 0;
							for (int i = 0;i < tokens.size();i++)
							{
								std::string typeStr, nameStr;
								getTypeAndName(tokens[i], typeStr, nameStr);
								auto typeID = getTypeIndex(typeStr.c_str());
								c->param[i].typeIndex = typeID;
								c->param[i]._dim = typeID == -1 ? 1 : typeDesc[typeID]._dim;
								c->param[i]._min = typeID == -1 ? INT_MIN : typeDesc[typeID]._min;
								c->param[i]._max = typeID == -1 ? INT_MAX : typeDesc[typeID]._max;

								SetParamType(i, typeStr.c_str());
								SetParamName(i, nameStr.c_str());
								c->param[i].offset = param_ofs;
								param_ofs += c->param[i].size;
							}

							c->pCount = tokens.size();

							break;
						}
					}

					lc++;
				}

				ifile.close();
			}
		

			//get caller string
			std::ifstream ifileCaller(caller.file_name());

			std::string funcStr;
			lc = 1;

			if (ifileCaller.is_open())
			{
				while (true)
				{
					if (!getline(ifileCaller, s)) break;
					if (lc == caller.line()) break;

					lc++;
				}
   
				auto ofs = s.rfind(".", caller.column())-1;
				while (ofs > 0 && std::isalnum(s.at(ofs))) ofs--;

				if (!std::isalnum(s.at(ofs))) ofs++;

				funcStr += s.c_str() + ofs;
				std::erase(funcStr,' ');

				if (!funcStr.find(";"))
				{
					while (true)
					{
						char a;
						ifileCaller.get(a);
						funcStr += a;
						if (a == ';') break;
					}
				}

				std::erase(funcStr,' ');
				std::erase(funcStr,'\t');
				std::erase(funcStr,'\n');
			
				auto br = funcStr.find('(');

				funcName = funcStr.substr(0, br);
				strcpy(c->funcName, funcName.c_str());

				auto funcStrEnd = funcStr.find(";");
				if (funcStrEnd < funcStr.size()) funcStrEnd++;
				funcStr.erase(funcStrEnd, funcStr.size() - funcStrEnd);
				std::string paramStr = funcStr.substr(br+2, funcStr.size()-3 - (br + 2));

				const std::regex reg{ R"(,)" };
				auto pTokens = regex_split(paramStr, reg);

				//TODO - prevent missmatch pTokens.size with pCount
				for (int i = 0; i < pTokens.size(); i++)
				{
					std::string pname = pTokens[i].substr(1, pTokens[i].find('=')-1);
					auto pId = getParamIndexByStr(cmdCounter, pname.c_str());
					std::string pvalue = pTokens[i].substr(pTokens[i].find('=') + 1, pTokens[i].size());
					
					c->param[i].bypass = false;

					if (isTypeEnum(cmdParamDesc[cmdCounter].param[pId].typeIndex))
					{
						std::string pTypeStr = cmdParamDesc[cmdCounter].param[pId].type;
						pTypeStr += "::";
						if (pvalue.find(pTypeStr) == std::string::npos)
						{
							c->param[i].bypass = true;
						}
					}
					else
					{
						if (!isParam(pvalue)) {
							c->param[i].bypass = true;

							for (int j = 0; j < c->param[i]._dim; j++)
							{
								c->param[i].value[j] = *(int*)((char*)in + c->param[i].offset + sizeof(int) * j);
							}
						}
						else
						{
							c->param[i].value[0] = std::stoi(pvalue);
						}

					}
				}

				ifileCaller.close();

				/*auto c = &cmdParamDesc[cmdCounter];
				
		

				for (int i = 0; i < cmdParamDesc[cmdCounter].pCount; i++)
				{
					for (int j=0;j< c->param[i]._dim;j++)
					{
						c->param[i].value[j] = *(int*)((char*)in + c->param[i].offset + sizeof(int)*j);
					}
				}*/
			}
		}
		else//variables <- reflected struct
		{
			bool changed = false;

			for (int i = 0; i < c->pCount; i++)
			{
				auto dim = c->param[i]._dim;
				for (int j = 0; j < dim; j++)
				{
					if (*(int*)((char*)in + c->param[i].offset + sizeof(int) * j) != c->param[i].value[j])
					{
						changed = true;
					}

					*(int*)((char*)in + c->param[i].offset + sizeof(int) * j) = c->param[i].value[j];
				}
			}

			if (changed && currentCmd != cmdCounter)
			{
				SaveToSource();
			}
		}

		c->stackLevel = cmdLevel;
		c->uiDraw = &editor::paramEdit::showStackItem;
		cmdLevel++;

		cmdCounter++;

	}

#define reflect reflect_f(&in, caller, std::source_location::current())
#define reflect_close cmdLevel--
#define prm(type,name) type,name

	struct object1 {

		#pragma pack(push, 1)
		struct params {
			int        x; 
			int y    ;
			       int z;  
			texture target;
		};
		#pragma pack(pop)

		void set(params in CALLER)
		{

			

		reflect;

			int c = 0;


		reflect_close;
		};
	
	} ;

	
	object1 obj1;

	void mainLoop()
	{
	
		
			
		//obj.set({0,20,{55,66,77},texture::mainRT});

		

		//obj::set(22, 20, texture::mainRT);

		//obj (bla, 10,20,texture::mainRT );

		

		BasicCam::camPass = false;
		BasicCam::camCounter = 0;

		if (!isPrecalc)
		{
			Precalc();
		}

		cmdCounter = precalcOfs;

		frameConst();

		obj1.set(211, 2, 212, texture::mainRT);


		
		gfx::SetInputAsm(topology::triList);
		gfx::SetBlendMode(blendmode::off, blendop::add);

		Cubemap::CalcCubemap(texture::env);
		Object::CalcObject(texture::obj1pos, texture::obj1nrml);

		gfx::SetRT(texture::mainRT, 0);
		gfx::ClearRT(255, 255, 255, 255);

		cameraMan();

		Cubemap::ShowCubemap(texture::env);

		gfx::SetDepthMode(depthmode::on);
		gfx::ClearRTDepth();
		gfx::SetCull(cullmode::back);
		gfx::SetBlendMode(blendmode::off, blendop::add);
		
		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 0, 120, 82, 112);
		
		

		//gfx::SetRT(texture::mainRT, 0);
		//gfx::ClearRT(0, 0, 0, 255);
		//tracker::Track(120);

		//gApi.ClearRT(0, 0, 0, 0);

	}

}
