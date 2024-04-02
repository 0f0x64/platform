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
		rtt,
		tex1,
		tex2
	};

	void Init()
	{
		CreateShaders();
		Textures::Create(texList::rtt, Textures::tType::flat, Textures::tFormat::u8, XMFLOAT2(1024, 1024), true,false);
		Textures::Create(texList::tex1, Textures::tType::flat, Textures::tFormat::u8, XMFLOAT2(1024, 1024), true, false);
		isInit = true;
	}

	void mainLoop()
	{
		if (!isInit) Init();

		Depth::SetDepthBuffer();
		CB::Update();
		CB::Set();

		SetIA();

		//dx::context->PSSetShaderResources(0, 0, NULL);
		ID3D11ShaderResourceView* const null[128] = { NULL };
		//context->PSSetShaderResources(0, 128, null);

		SetRT(0);
		
		ClearRT(0,0.f, 0.f, .15f, 1.f);

		Shaders::SetVS(Shaders::vertex::quad);
		Shaders::SetPS(Shaders::pixel::simple);
		NullDrawer(1, 1);


		SetRT2Screen();
		Depth::SetDepthBuffer();
		Clear(1.0f,0.f,.15f,1.f);

		Textures::Set(texList::rtt, 0);
		Shaders::SetVS(Shaders::vertex::quad2);
		Shaders::SetPS(Shaders::pixel::simple2);

		NullDrawer(1, 1);

		Present();

	}

}