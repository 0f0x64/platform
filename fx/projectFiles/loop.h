namespace Loop
{
	bool isInit = false;

	
	void Init()
	{
		dx::CompileVertexShaderFromFile(&dx::VS[0], L"../fx/projectFiles/quad.hlsl");
		dx::CompilePixelShaderFromFile(&dx::PS[0], L"../fx/projectFiles/simple.hlsl");
		isInit = true;
	}

	void mainLoop()
	{
		if (!isInit) Init();

		//	double t = timer::frameBeginTime * .01;
		dx::SetRT();
		dx::SetZBuffer();
		dx::Clear(XMVECTORF32{ .3f,.3f,.3f, 1.f });

		dx::NullDrawer(0, 1, 1);

		dx::Present();

	}

}