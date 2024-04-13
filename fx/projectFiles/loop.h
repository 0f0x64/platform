namespace Loop
{
	
	bool isPrecalc = false;

	#include "..\generated\constBufReflect.h"

	void Precalc()
	{
		InputAssembler::Set();
		ConstBuf::Update(5, ConstBuf::global);

		for (int i = 0; i < 6; i++) { ConstBuf::SetV(i); ConstBuf::SetP(i); }
		isPrecalc = true;
	}

	void frameConst()
	{
		ConstBuf::frame.time = (float)(timer::frameBeginTime * .01);
		ConstBuf::UpdateFrame();
	}
	/*
	void Scene1()
	{
		//two dynamic texures
		api.cull(cullmode::off);
		api.rt(tex::tex1);
		api.depth(depthmode::off);
			
			vs::quad.set();

			ps::simpleFx.params.r = 0;
			ps::simpleFx.params.g = 1;
			ps::simpleFx.params.b = 0;
			ps::simpleFx.set();
		
		api.draw(1, 1);
		api.mips();

		api.copyColor(tex::tex2, tex::tex1);
		/*api.rt(tex::tex2);

			ps::simpleFx.params.r = 1;
			ps::simpleFx.params.g = 0;
			ps::simpleFx.params.b = 0;
			ps::simpleFx.set();

		api.draw(1, 1);
		api.mips();

		//-two planes
		api.cull(cullmode::off);
		api.rt(tex::mainRT);
		api.cam();
		api.depth(depthmode::on);

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
	*/
	void ShowCubemap()
	{
		api.blend(blendmode::off, blendop::add);
		api.cull(cullmode::back);
		api.rt(tex::mainRT);
		api.cam();
		api.depth(depthmode::on);
		api.clear(0.2f, 0.2f, 0.2f, 1.f);
		api.clearDepth();

		ps::cubeMapViewer.textures.env = tex::env;
		ps::cubeMapViewer.samplers.sam1Filter = filter::linear;
		ps::cubeMapViewer.samplers.sam1AddressU = addr::wrap;
		ps::cubeMapViewer.samplers.sam1AddressV = addr::wrap;
		ps::cubeMapViewer.set();
		int gX = 4;
		int gY = 3;
		vs::simpleCube.params.gX = (float)gX;
		vs::simpleCube.params.gY = (float)gY;
		vs::simpleCube.set();
		api.draw(1, gX*gY);

	}

	void ShowObject()
	{
		api.blend(blendmode::off, blendop::add);
		api.cull(cullmode::back);
		api.rt(tex::mainRT);
		api.cam();
		api.depth(depthmode::on);

		float gX = Textures::texture[tex::obj1pos].size.x;
		float gY = Textures::texture[tex::obj1pos].size.y;
		vs::objViewer.textures.positions = tex::obj1pos;
		vs::objViewer.samplers.sam1Filter = filter::linear;
		vs::objViewer.samplers.sam1AddressU = addr::wrap;
		vs::objViewer.samplers.sam1AddressV = addr::wrap;
		vs::objViewer.params.gX = (float)gX;
		vs::objViewer.params.gY = (float)gY;
		vs::objViewer.set();

		ps::basic.textures.env = tex::env;
		ps::basic.textures.normals = tex::obj1nrml;
		ps::basic.samplers.sam1Filter = filter::linear;
		ps::basic.samplers.sam1AddressU = addr::wrap;
		ps::basic.samplers.sam1AddressV = addr::wrap;
		ps::basic.set();
		api.draw(1, (int)gX * (int)gY);
	}

	void CalcCubemap()
	{
		api.blend(blendmode::off, blendop::add);
		api.rt(tex::env);
		api.cull(cullmode::off);
		api.depth(depthmode::off);
		api.clearDepth();
		api.clear(sin(timer::GetCounter()*.01), 0, 0, 1);

		vs::quad.set();
		ps::cubemapCreator.set();
		api.draw(1, 1);
		api.mips();
	}

	void CalcObject()
	{
		api.blend(blendmode::off, blendop::add);
		api.cull(cullmode::off);
		api.rt(tex::obj1pos);
		api.depth(depthmode::off);

		//pos
		vs::quad.set();
		ps::obj1.set();
		api.draw(1, 1);
		api.mips();

		//normals
		api.rt(tex::obj1nrml);
		vs::quad.set();

		ps::genNormals.samplers.sam1Filter = filter::linear;
		ps::genNormals.samplers.sam1AddressU = addr::wrap;
		ps::genNormals.samplers.sam1AddressV = addr::wrap;

		ps::genNormals.textures.geo = tex::obj1pos;
		ps::genNormals.set();
		api.draw(1, 1);
		api.mips();
	}


	void mainLoop()
	{
		frameConst();

		if (!isPrecalc) Precalc();

		CalcCubemap();

		CalcObject();

		ShowCubemap();
		ShowObject();

		Textures::UnbindAll();
		Draw::Present();

	}

}
