namespace Loop
{
	bool isInit = false;

#if EditMode

	void CreateShaders()
	{
		shaders::vsCount = sizeof(shaders::vsList) / sizeof(const char*);
		int i = 0;
		while (i<shaders::vsCount)
		{
			char fileName[255];
			strcpy(fileName, "/vs/");
			strcat(fileName, shaders::vsList[i]);
			strcat(fileName, ".hlsl");
			dx::Shaders::Compiler::Vertex(i, fileName);
			i++;
			
		}

		shaders::psCount = sizeof(shaders::psList) / sizeof(const char*);
		i = 0;
		while (i < shaders::psCount)
		{
			char fileName[255];
			strcpy(fileName, "/ps/");
			strcat(fileName, shaders::psList[i]);
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

	void Init()
	{
		CreateShaders();

		isInit = true;
	}

	void mainLoop()
	{
		if (!isInit) Init();

		//	double t = timer::frameBeginTime * .01;
		dx::SetIA();
		dx::SetRT();
		dx::Depth::SetDepthBuffer();
		dx::Clear(XMVECTORF32{ .3f,.3f,.3f, 1.f });

		dx::CB::Set();

		dx::Shaders::SetVS(shaders::vertex::quad);
		dx::Shaders::SetPS(shaders::pixel::simple);
		dx::NullDrawer(1, 1);

		dx::Shaders::SetVS(shaders::vertex::quad2);
		dx::Shaders::SetPS(shaders::pixel::simple2);
		dx::NullDrawer(1, 1);

		dx::Present();

	}

}