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
		api.rt(tex::tex1);
		api.depth(depthmode::off);
		vs::quad.set();
		ps::simpleFx.params.r = 0;
		ps::simpleFx.params.g = 1;
		ps::simpleFx.params.b = 0;
		ps::simpleFx.set();
		api.draw(1, 1);
		api.mips();

		api.rt(tex::tex2);
		ps::simpleFx.params.r = 1;
		ps::simpleFx.params.g = 0;
		ps::simpleFx.params.b = 0;
		ps::simpleFx.set();
		api.draw(1, 1);
		api.mips();

		//-two planes
		api.rt(tex::mainRT);
		api.cam();
		api.depth(Depth::mode::on);

		ps::simpleTex.samplers.sam1Filter = filter::linear;
		ps::simpleTex.samplers.sam1AddressU = addr::wrap;
		ps::simpleTex.samplers.sam1AddressV = addr::wrap;

		api.clear(0.2f, 0.2f, 0.2f, 1.f);
		api.clearDepth();

		vs::meshOut.params.tone = 1;
		vs::meshOut.set();
		ps::simpleTex.textures.tex1 = tex::tex1;
		ps::simpleTex.textures.tex2 = tex::tex2;
		ps::simpleTex.params.mix = 0;
		ps::simpleTex.set();
		api.blend(blendmode::off, blendop::add);
		api.draw(1, 1);

		ps::simpleTex.params.mix = 1;
		ps::simpleTex.set();
		vs::meshOut.params.tone = 0;
		vs::meshOut.set();
		api.draw(1, 1);
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