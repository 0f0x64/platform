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

	//shortcuts

	//resources
	#define tex Textures::list
	#define rt Textures::RenderTargets::Api
	#define cb ConstBuf::Api
	#define vs Shaders::vertex
	#define ps Shaders::pixel
	
	
	#define depth Depth::Api
	#define depthmode Depth::Mode
	#define depthOff depth.Set(depthmode::off);
	#define depthOn depth.Set(depthmode::on);
	#define depthR depth.Set(depthmode::readonly);
	#define depthW depth.Set(depthmode::writeonly);

	#define texture Textures::Api
	#define shader Shaders::Api
	#define draw Draw
	#define sampler Sampler::Api

	#define pSamplerLU(slot) sampler.Set(Sampler::to::pixel, slot , Sampler::type::Linear, Sampler::addr::wrap, Sampler::addr::clamp)
	#define pSamplerLV(slot) sampler.Set(Sampler::to::pixel, slot , Sampler::type::Linear, Sampler::addr::clamp, Sampler::addr::wrap)
	#define pSamplerLUV(slot) sampler.Set(Sampler::to::pixel, slot , Sampler::type::Linear, Sampler::addr::wrap, Sampler::addr::wrap)

	void mainLoop()
	{
		
		IA.Set();

		if (!isInit) Init();

		cb.Update();
		cb.Set();

		//depth.Set(depthmode::off);
		depthOff;

		rt.Set(tex::tex1);

		draw.Clear(0.f, 0.f, .15f, 1.f);
		shader.Set(vs::quad,ps::simple);
		draw.NullDrawer(1, 1);
		rt.CreateMipMap();

		rt.Set(tex::mainRT);
		draw.Clear(1.0f,0.f,.15f,1.f);
		
		texture.Set(tex::tex1, 0);
		shader.Set(vs::quad2, ps::simple3);
		pSamplerLUV(0);

		draw.NullDrawer(1, 1);

		draw.Present();

	}

}