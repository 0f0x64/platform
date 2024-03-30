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
	namespace shadersI {
		#include "generated\quad.h"
		#include "generated\simple.h"
	};
#endif


#if EditMode
	void CreateShaders()
	{
		int i = 0;
		while (*shaderNameList[i] != 0)
		{
			dx::CompileVertexShaderFromFile(&dx::VS[vsCounter++], shaderNameList[i]);
			dx::CompilePixelShaderFromFile(&dx::PS[psCounter++], shaderNameList[i]);
		}
	}
#else
	void CreateShaders()
	{
		auto i = shaders::0;

		/*int i = 0;
		while (*shaderNameList[i] != 0)
		{
			dx::CompileVertexShaderFromFile(&dx::VS[vsCounter++], shaders[i]);
			dx::CompilePixelShaderFromFile(&dx::PS[psCounter++], shaders[i]);
		}*/
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
		dx::SetVS(shaders::simple);

		dx::NullDrawer(1, 1);

		dx::Present();

	}

}