namespace Loop
{
	bool isInit = false;

	int vsCounter = 0;
	int psCounter = 0;

	#define shader(VariableName) VariableName
	enum shaders {
		#include "projectFiles\shaders.h"
	};

#if EditMode
	#undef shader
	#define shader(VariableName) # VariableName
	const char* shaderNameList[] = {
		#include "projectFiles\shaders.h"
	};

#else
		#include "generated\shaders.h"
#endif


#if EditMode
	void CreateShaders()
	{
		int shadersCount = sizeof(shaderNameList) / sizeof(const char*);
		int i = 0;
		while (i<shadersCount)
		{
			dx::CompileVertexShaderFromFile(&dx::VS[vsCounter++], shaderNameList[i]);
			dx::CompilePixelShaderFromFile(&dx::PS[psCounter++], shaderNameList[i]);
		}
	}
#else
	void CreateShaders()
	{
		shadersData::CompileAll();
	}

#endif

	void Init()
	{
		vsCounter = 0;
		psCounter = 0;

		CreateShaders();

		isInit = true;
	}

	void mainLoop()
	{
		if (!isInit) Init();

		//	double t = timer::frameBeginTime * .01;
		dx::SetIA();
		dx::SetRT();
		dx::SetZBuffer();
		dx::Clear(XMVECTORF32{ .3f,.3f,.3f, 1.f });

		dx::SetCB();

		dx::SetVS(shaders::quad);
		dx::SetPS(shaders::simple);


		dx::NullDrawer(1, 1);

		dx::Present();

	}

}