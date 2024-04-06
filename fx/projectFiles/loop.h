namespace Loop
{
	bool isInit = false;
	bool isPrecalc = false;

	using namespace dx;

	//shortcuts
	#define tex Textures::list
	#define vs Shaders::vertex
	#define ps Shaders::pixel
	
	#define rt Textures::RenderTargets::Api.Set
	#define createMips Textures::Api.CreateMipMap
	
	#define blend Blend::Set
	#define blendMode Blend::mode
	#define blendOp Blend::op

	#define depth Depth::Api
	#define texture Textures::Api.Set
	#define shader Shaders::Api.Set
	#define draw Draw
	#define sampler Sampler::Api.Set
	#define ia InputAssembler

	#define constant ConstBuf
	#define cb ConstBuf::Api

	#define pSampler_L_U(slot) sampler(Sampler::to::pixel, slot ,  Sampler::type::Linear, Sampler::addr::wrap, Sampler::addr::clamp)
	#define pSampler_L_V(slot) sampler(Sampler::to::pixel, slot ,  Sampler::type::Linear, Sampler::addr::clamp, Sampler::addr::wrap)
	#define pSampler_L_UV(slot) sampler(Sampler::to::pixel, slot , Sampler::type::Linear, Sampler::addr::wrap, Sampler::addr::wrap)
	 
	#define pSampler_P_U(slot) sampler(Sampler::to::pixel, slot ,  Sampler::type::Point, Sampler::addr::wrap, Sampler::addr::clamp)
	#define pSampler_P_V(slot) sampler(Sampler::to::pixel, slot ,  Sampler::type::Point, Sampler::addr::clamp, Sampler::addr::wrap)
	#define pSampler_P_UV(slot) sampler(Sampler::to::pixel, slot , Sampler::type::Point, Sampler::addr::wrap, Sampler::addr::wrap)

	#define pSampler_PL_U(slot) sampler(Sampler::to::pixel, slot ,  Sampler::type::MinPointMagLinear, Sampler::addr::wrap, Sampler::addr::clamp)
	#define pSampler_PL_V(slot) sampler(Sampler::to::pixel, slot ,  Sampler::type::MinPointMagLinear, Sampler::addr::clamp, Sampler::addr::wrap)
	#define pSampler_PL_UV(slot) sampler(Sampler::to::pixel, slot , Sampler::type::MinPointMagLinear, Sampler::addr::wrap, Sampler::addr::wrap)

	void Init()
	{
		Shaders::Init();
		Textures::Init();
		
		ia.Set();

		for (int i=0;i<5;i++) cb.Set(i);

		isInit = true;
	}

	void Precalc()
	{
		isPrecalc = true;
	}

	void mainLoop()
	{
		constant::frame[0] = XMFLOAT4((float)(timer::frameBeginTime * .01), 0, 0, 0);
		cb.UpdateFrame();

		if (!isInit) Init();
		if (!isPrecalc) Precalc();
	
		depth.Off();
		rt(tex::tex1);

		shader(vs::quad,ps::simple);
		draw.NullDrawer(1, 1);
		createMips();

		rt(tex::mainRT);
		draw.Clear(0.2f,0.2f,0.2f,1.f);

		texture(tex::tex1, 0);
		pSampler_L_UV(0);

		shader(vs::quad2, ps::simple3);
		constant::params[0] = XMFLOAT4(1, 0, 0, 1);
		cb.UpdateParams();
		blend(blendMode::on, blendOp::add);
		draw.NullDrawer(1, 1);

		shader(vs::quad3, ps::simple3);
		constant::params[0] = XMFLOAT4(0, 1, 0, 1);
		cb.UpdateParams();
		blend(blendMode::alpha, blendOp::add);
		draw.NullDrawer(1, 1);

		draw.Present();

	}

}