namespace Loop
{
	bool isInit = false;

	using namespace dx;

	void Init()
	{
		Shaders::Init();
		Textures::Init();
		isInit = true;
	}

	#define RT Textures::RenderTargets
	#define CB ConstBuf
	#define VS Shaders::vertex
	#define PS Shaders::pixel
	#define TEX Textures::list

	void mainLoop()
	{
		
		IA::Set();

		if (!isInit) Init();

		CB::Update();
		CB::Set();

		Depth::Set(Depth::Mode::off);

		RT::Set(TEX::tex1);
		Textures::UnbindAll();
		Draw::Clear(0.f, 0.f, .15f, 1.f);
		Shaders::Set(VS::quad,PS::simple);
		Draw::NullDrawer(1, 1);
		Textures::CreateMipMap();

		RT::Set(TEX::mainRT);
		Draw::Clear(1.0f,0.f,.15f,1.f);
		
		Textures::Set(TEX::tex1, 0);
		Shaders::Set(VS::quad2, PS::simple3);
		Sampler::Set(Sampler::to::ps, 0, Sampler::type::Linear, Sampler::addr::clamp, Sampler::addr::wrap);
		Draw::NullDrawer(1, 1);
		
		Draw::Present();

	}

}