
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

		int x = strlen(funcName) - 1;;
		for (x; x > 0; x--) { if (funcName[x] == '.') { x++; break; } };

		strcpy(cmdParamDesc[cmdCounter].funcName, funcName + x);
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

	void reflect_f(auto* in, const std::source_location caller, const std::source_location currentFunc)//name and types without names
	{
		int* rawData = (int*)in;

		if (!paramsAreLoaded)  //variables -> reflected struct
		{
			FillCaller(caller);

			std::string fn = currentFunc.function_name();
			auto rb = fn.find("(");
			auto fp = fn.rfind("::", rb);
			auto op = fn.rfind("::", fp - 2);
			std::string objName = fn.substr(op+2, fp - op - 2);
			std::string functionName = fn.substr(fp + 2, rb - fp - 2);

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
							
							for (int i = 0;i < tokens.size();i++)
							{
								std::string typeStr, nameStr;
								getTypeAndName(tokens[i], typeStr, nameStr);
								SetParamType(i, typeStr.c_str());
								SetParamName(i, nameStr.c_str());
							}

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

				funcStr.append(s);
		
				while (s.find(";") == std::string::npos)
				{
					getline(ifileCaller, s);
					funcStr.append(s);
				}
			
			
				ifileCaller.close();
			}

			//isolate caller
			unsigned int start = caller.column();
			while (true)
			{
				if (start == 0 ||
					funcStr.at(start) == ' ' ||
					funcStr.at(start) == '\t' ||
					funcStr.at(start) == ';')
				{
					start++;
					break;
				}

				start--;
			}
			unsigned int end = funcStr.find(";")+1;
			std::string funcStrClean = funcStr.substr(start, end - start);

			//find name
			unsigned nameEnd = start;
			while (true)
			{
				if (funcStr.at(nameEnd) == ' ' ||
					funcStr.at(nameEnd) == '(')
				{
					break;
				}
				nameEnd++;
			}

			std::string funcName = funcStr.substr(start, nameEnd- start);

			strcpy(cmdParamDesc[cmdCounter].funcName, funcName.c_str());

			//

			int pCount = sizeof(*in)/sizeof(int);
			auto c = &cmdParamDesc[cmdCounter];


			c->pCount = pCount;

			for (int i = 0; i < pCount; i++)
			{
				c->param[i].value[0] = *(rawData + i);
				c->param[i].bypass = false;
			}

			editor::paramEdit::setParamsAttr();
			//editor::paramEdit::setBypass();//logic change needed

		}
		else//variables <- reflected struct
		{
			auto c = &cmdParamDesc[cmdCounter];

			for (int i = 0; i < c->pCount; i++)
			{
				if (!c->param[i].bypass) *(rawData + i) = c->param[i].value[0];
			}


		}

		AddToUI2(cmdParamDesc[cmdCounter].funcName);
		cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
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

	
	object1 obj;

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

		obj.set({
			.x = 111,
			.y = 10,
			.z = 22,
			.target = texture::mainRT
			});

		
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
