namespace Loop
{
	bool isInit = false;

	using namespace dx;

#if EditMode

	void CreateShaders()
	{
		Shaders::vsCount = sizeof(Shaders::vsList) / sizeof(const char*);
		int i = 0;
		while (i<Shaders::vsCount)
		{
			char fileName[255];
			strcpy(fileName, "/vs/");
			strcat(fileName, Shaders::vsList[i]);
			strcat(fileName, ".hlsl");
			dx::Shaders::Compiler::Vertex(i, fileName);
			i++;
			
		}

		Shaders::psCount = sizeof(Shaders::psList) / sizeof(const char*);
		i = 0;
		while (i < Shaders::psCount)
		{
			char fileName[255];
			strcpy(fileName, "/ps/");
			strcat(fileName, Shaders::psList[i]);
			strcat(fileName, ".hlsl");
			dx::Shaders::Compiler::Pixel(i, fileName);
			i++;
		}

	}

#else

	#include "generated\processedShaders.h"

	void CreateShaders()
	{
		shadersData::CompileAll();
	}

#endif

	enum texList {
		mainRT,
		tex1,
		tex2
	};

	void Init()
	{
		CreateShaders();
		Textures::Create(texList::mainRT, Textures::tType::flat, Textures::tFormat::u8, XMFLOAT2((float)dx::width, (float)dx::height), false,true);
		Textures::Create(texList::tex1, Textures::tType::flat, Textures::tFormat::u8, XMFLOAT2(1024, 1024), true, false);
		Textures::Create(texList::tex2, Textures::tType::flat, Textures::tFormat::u8, XMFLOAT2(1024, 1024), true, false);
		
		isInit = true;
	}

	#define RT Textures::RenderTargets
	#define CB ConstBuf
	#define VS Shaders::vertex
	#define PS Shaders::pixel
	#define TEX texList

	void mainLoop()
	{
		SetIA();

		if (!isInit) Init();

		CB::Update();
		CB::Set();

		Textures::UnbindAll();
		Depth::Set(Depth::Mode::off);

		RT::Set(TEX::tex1);
		Draw::Clear(0.f, 0.f, .15f, 1.f);
		Shaders::Set(VS::quad,PS::simple);
		Draw::NullDrawer(1, 1);
		Textures::CreateMipMap();

		RT::Set(TEX::mainRT);
		Draw::Clear(1.0f,0.f,.15f,1.f);
		
		Textures::Set(TEX::tex1, 0);
		Shaders::Set(VS::quad2, PS::simple2);
		Sampler::Set(Sampler::to::ps, 0, Sampler::type::Linear, Sampler::addr::clamp, Sampler::addr::wrap);
		Draw::NullDrawer(1, 1);
		
		Present();

	}

}