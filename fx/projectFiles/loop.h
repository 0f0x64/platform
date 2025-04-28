
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

	void reflect_f(auto* in, const std::source_location caller, const std::source_location currentFunc)//name and types without names
	{
		int* rawData = (int*)in;

		{
			std::string fn = currentFunc.function_name();
			auto rb = fn.find("(");
			auto fp = fn.rfind("::",rb);
			auto op = fn.rfind("::", fp-2);
			std::string objName = fn.substr(op+2, fp - op-2);


			std::ifstream ifile(currentFunc.file_name());
			std::string s;
			std::string funcStr;
			int lc = 1;

			bool obj_is_found = false;

			if (ifile.is_open())
			{
				while (true)
				{
					if (!getline(ifile, s)) break;
					if (lc == currentFunc.line()) break;

					if (s.find("struct") != std::string::npos)
					{
						if (s.find(objName.c_str()) != std::string::npos)
						{
							obj_is_found = true;
						}
					}

					if (obj_is_found)
					{
						unsigned int fs = s.find("struct");
						if ( fs != std::string::npos)
						{
							unsigned int fp = s.find("params",fs);
							if (fp != std::string::npos)
							{
								unsigned int fb = s.find("{",fp);
								if (fb != std::string::npos) 
								{
									std::string pStr;
									while (true)
									{
										char a;
										ifile.get(a);
										if (a == '}') break;
										pStr += a;
									}

									int t = 0;

								}
							}
						}
					}

					lc++;
				}

				ifile.close();
			}
		}

		//get caller string
		std::string inFilePath = caller.file_name();
		std::ifstream ifile(inFilePath);

		std::string s;
		std::string funcStr;
		int lc = 1;

		if (ifile.is_open())
		{
			while (true)
			{
				if (!getline(ifile, s)) break;
				if (lc == caller.line()) break;

				lc++;
			}

			funcStr.append(s);
		
			while (s.find(";") == std::string::npos)
			{
				getline(ifile, s);
				funcStr.append(s);
			}
			
			
			ifile.close();
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



		//

		int pCount = sizeof(*in)/sizeof(int);
		auto c = &cmdParamDesc[cmdCounter];

		if (paramsAreLoaded) //variables <- reflected struct
		{
			for (int i = 0; i < pCount; i++)
			{
				if (!c->param[i].bypass) *(rawData + i) = c->param[i].value[0];
			}
		
		}
		else //variables -> reflected struct
		{
			strcpy(c->caller.fileName, caller.file_name());
			c->caller.line = caller.line();
			c->caller.column = caller.column();
			c->pCount = pCount;
			 
			for (int i = 0; i < pCount; i++)
			{
				c->param[i].value[0] = *(rawData + i);
				c->param[i].bypass  = false;
				strcpy(c->param[i].type, "type");
				strcpy(c->param[i].type, "name");

			}

			editor::paramEdit::setParamsAttr();
			//editor::paramEdit::setBypass();//logic change needed

		}

		AddToUI2(funcName.c_str());
		cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
		cmdLevel++;

		cmdCounter++;

	}

#define reflect reflect_f(&in, caller, std::source_location::current())
#define reflect_close cmdLevel--

	struct object1 {

		#pragma pack(push, 1)
		struct params {
			int x; 
			int y;
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
			.x = 0,
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
