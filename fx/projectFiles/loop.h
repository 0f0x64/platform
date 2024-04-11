namespace Loop
{
	
	bool isPrecalc = false;

	using namespace dx;

	#include "..\generated\constBufReflect.h"

	void param(int param, float value)
	{
		ConstBuf::drawerV[param] = value;
	}

	void Precalc()
	{
		InputAssembler::Set();
		ConstBuf::Update(5, ConstBuf::global);

		for (int i = 0; i < 6; i++) ConstBuf::SetVP(i);
		isPrecalc = true;
	}

	void frameConst()
	{
		ConstBuf::frame.time = (float)(timer::frameBeginTime * .01);
		ConstBuf::UpdateFrame();
	}

		
	void Scene1()
	{
		Depth::Off();
		Textures::SetRT(Textures::list::tex1);
		Shaders::Set(Shaders::vertex::quad, Shaders::pixel::simpleFx);
		Draw::NullDrawer(1, 1);
		Textures::CreateMipMap();

		Textures::SetRT(Textures::list::mainRT);
		Camera::Set();
		Depth::On();
		Draw::Clear(0.2f, 0.2f, 0.2f, 1.f);
		Draw::ClearDepth();

		Textures::SetTexture(Textures::list::tex1, 0);
		Sampler::Set(Sampler::to::pixel, 0, Sampler::type::Linear, Sampler::addr::wrap, Sampler::addr::wrap);

		Shaders::Set(Shaders::vertex::meshOut, Shaders::pixel::simpleTex);
		ConstBuf::drawerV[0] = 1;
		ConstBuf::drawerP[0] = 1;

		Blend::Set(Blend::mode::off, Blend::op::add);
		Draw::NullDrawer(1, 1);

		Shaders::Set(Shaders::vertex::meshOut, Shaders::pixel::simpleTex);
		ConstBuf::drawerV[0] = 0;
		ConstBuf::drawerP[0] = 0;
		Blend::Set(Blend::mode::off, Blend::op::add);

		Draw::NullDrawer(1, 1);
	}

	void Scene2()
	{
		Textures::SetRT(Textures::list::mainRT);
		Blend::Set(Blend::mode::off, Blend::op::add);
		Draw::Clear(1.2f, 0.2f, 0.2f, 1.f);
		Depth::Off();
		Shaders::Set(Shaders::vertex::quad, Shaders::pixel::simpleFx);
		Draw::NullDrawer(1, 1);

	}

	void mainLoop()
	{
		frameConst();

		if (!isPrecalc) Precalc();

		Scene1();

		Textures::UnbindAll();
		Draw::Present();

	}

}