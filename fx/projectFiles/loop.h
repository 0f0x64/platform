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
	#define tex Textures::list
	#define rt Textures::RenderTargets::Api.Set
	#define createMips Textures::RenderTargets::Api.CreateMipMap()
	#define cb ConstBuf::Api
	#define vs Shaders::vertex
	#define ps Shaders::pixel
	#define depth Depth::Api
	#define texture Textures::Api.Set
	#define shader Shaders::Api.Set
	#define draw Draw
	#define sampler Sampler::Api.Set
	#define ia InputAssembler

	#define pSampler_L_U(slot) sampler(Sampler::to::pixel, slot ,  Sampler::type::Linear, Sampler::addr::wrap, Sampler::addr::clamp)
	#define pSampler_L_V(slot) sampler(Sampler::to::pixel, slot ,  Sampler::type::Linear, Sampler::addr::clamp, Sampler::addr::wrap)
	#define pSampler_L_UV(slot) sampler(Sampler::to::pixel, slot , Sampler::type::Linear, Sampler::addr::wrap, Sampler::addr::wrap)
	 
	#define pSampler_P_U(slot) sampler(Sampler::to::pixel, slot ,  Sampler::type::Point, Sampler::addr::wrap, Sampler::addr::clamp)
	#define pSampler_P_V(slot) sampler(Sampler::to::pixel, slot ,  Sampler::type::Point, Sampler::addr::clamp, Sampler::addr::wrap)
	#define pSampler_P_UV(slot) sampler(Sampler::to::pixel, slot , Sampler::type::Point, Sampler::addr::wrap, Sampler::addr::wrap)

	#define pSampler_PL_U(slot) sampler(Sampler::to::pixel, slot ,  Sampler::type::MinPointMagLinear, Sampler::addr::wrap, Sampler::addr::clamp)
	#define pSampler_PL_V(slot) sampler(Sampler::to::pixel, slot ,  Sampler::type::MinPointMagLinear, Sampler::addr::clamp, Sampler::addr::wrap)
	#define pSampler_PL_UV(slot) sampler(Sampler::to::pixel, slot , Sampler::type::MinPointMagLinear, Sampler::addr::wrap, Sampler::addr::wrap)


	void mainLoop()
	{
		
		ia.Set();

		if (!isInit) Init();

		cb.Update();
		cb.Set();

		depth.Off();

		rt(tex::tex1);

		draw.Clear(0.f, 0.f, .15f, 1.f);
		shader(vs::quad,ps::simple);
		draw.NullDrawer(1, 1);
		createMips;

		rt(tex::mainRT);
		draw.Clear(0.2f,0.2f,0.2f,1.f);

		texture(tex::tex1, 0);
		pSampler_L_UV(0);

		shader(vs::quad2, ps::simple3);
		dx::Blend::Set(dx::Blend::mode::on, dx::Blend::op::add);
		draw.NullDrawer(1, 1);

		shader(vs::quad3, ps::simple3);
		dx::Blend::Set(dx::Blend::mode::alpha, dx::Blend::op::add);
		draw.NullDrawer(1, 1);


		draw.Present();

	}

}