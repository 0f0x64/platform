namespace Loop
{
	
	bool isPrecalc = false;

	using namespace dx;

	#include "..\generated\constBufReflect.h"

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
		//two dynamic texures
		Textures::SetRT(Textures::list::tex1);
		Depth::Set(Depth::mode::off);
		quad.set();
		simpleFx.params.r = 0;
		simpleFx.params.g = 1;
		simpleFx.params.b = 0;
		simpleFx.set();
		Draw::NullDrawer(1, 1);
		Textures::CreateMipMap();

		Textures::SetRT(Textures::list::tex2);
		simpleFx.params.r = 1;
		simpleFx.params.g = 0;
		simpleFx.params.b = 0;
		simpleFx.set();
		Draw::NullDrawer(1, 1);
		Textures::CreateMipMap();

		//-two planes
		Textures::SetRT(Textures::list::mainRT);
		Camera::Set();
		Depth::Set(Depth::mode::on);

		simpleTex.samplers.sam1Filter = Sampler::type::linear;
		simpleTex.samplers.sam1AddressU = Sampler::addr::wrap;
		simpleTex.samplers.sam1AddressV = Sampler::addr::wrap;

		Draw::Clear(0.2f, 0.2f, 0.2f, 1.f);
		Draw::ClearDepth();

		meshOut.params.tone = 1;
		meshOut.set();
		simpleTex.textures.tex1 = Textures::list::tex1;
		simpleTex.textures.tex2 = Textures::list::tex2;
		simpleTex.params.mix = 0;

		simpleTex.set();
		Blend::Set(Blend::mode::off, Blend::op::add);
		Draw::NullDrawer(1, 1);

		meshOut.params.tone = 0;
		meshOut.set();
		simpleTex.textures.tex1 = Textures::list::tex1;
		simpleTex.textures.tex2 = Textures::list::tex2;
		simpleTex.params.mix = 1;
		simpleTex.set();
		Blend::Set(Blend::mode::off, Blend::op::add);
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