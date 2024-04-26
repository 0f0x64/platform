namespace Loop
{
	
	#include "uitils.h"
	#include "camera.h"

	bool isPrecalc = false;



	//----------

	

	void Precalc()
	{


		//---

		api.setIA(topology::triList);
		ConstBuf::Update(5, ConstBuf::global);

		for (int i = 0; i < 6; i++) { ConstBuf::SetToVertex(i); ConstBuf::SetToPixel(i); }
		isPrecalc = true;
	}

	void frameConst()
	{
		ConstBuf::frame.time = (float)(timer::frameBeginTime * .01);
		ConstBuf::UpdateFrame();
	}
	
	void ShowCubemap(int envTex)
	{
		api.blend(blendmode::off, blendop::add);
		api.cull(cullmode::back);

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
		api.draw(gX*gY);

	}

	void ShowObject(int geometry, int normals, int quality)
	{
		int denom = (int)pow(2, quality);

		float gX = Textures::texture[tex::obj1pos].size.x/denom;
		float gY = Textures::texture[tex::obj1pos].size.y/denom;

		vs::objViewer.textures.positions = geometry;
		vs::objViewer.samplers.sam1Filter = filter::linear;
		vs::objViewer.samplers.sam1AddressU = addr::wrap;
		vs::objViewer.samplers.sam1AddressV = addr::clamp;
		vs::objViewer.params.gX = gX;
		vs::objViewer.params.gY = gY;
		vs::objViewer.set();

		ps::basic.textures.env = tex::env;
		ps::basic.textures.normals = normals;
		ps::basic.samplers.sam1Filter = filter::linear;
		ps::basic.samplers.sam1AddressU = addr::wrap;
		ps::basic.samplers.sam1AddressV = addr::wrap;

		ps::basic.set();
		api.draw((int)gX * (int)gY);
	}

	void CalcCubemap(int targetTexture)
	{
		api.blend(blendmode::off, blendop::add);
		api.rt(targetTexture);
		api.cull(cullmode::off);
		api.depth(depthmode::off);
		api.clear(sinf((float)timer::GetCounter()*.01f), 0, 0, 1);

		vs::quad.set();
		ps::cubemapCreator.set();
		api.draw(1, 1);
		api.mips();
	}

	void CalcObject(int targetGeoTexture,int targetNrmlTexture)
	{
		api.blend(blendmode::off, blendop::add);
		api.cull(cullmode::off);
		api.rt(targetGeoTexture);
		api.depth(depthmode::off);

		//pos
		vs::quad.set();
		ps::obj1.set();
		api.draw(1);
		api.mips();

		//normals
		api.rt(targetNrmlTexture);
		vs::quad.set();

		ps::genNormals.samplers.sam1Filter = filter::linear;
		ps::genNormals.samplers.sam1AddressU = addr::wrap;
		ps::genNormals.samplers.sam1AddressV = addr::wrap;

		ps::genNormals.textures.geo = targetGeoTexture;
		ps::genNormals.set();
		api.draw(1);
		api.mips();
	}



	


	void mainLoop()
	{
		regCam();

		frameConst();

		if (!isPrecalc) Precalc();

		api.setIA(topology::triList);

		CalcCubemap(tex::env);
		CalcObject(tex::obj1pos,tex::obj1nrml);

		api.blend(blendmode::off, blendop::add);
		api.rt(tex::mainRT);

		//setCam(0);
		float t = (timer::timeCursor / (SAMPLES_IN_FRAME * 60.f));
		slideCam(0, 1, t);

		api.depth(depthmode::on);
		api.clear(0.2f, 0.2f, 0.2f, 1.f);
		api.clearDepth();

		ShowCubemap(tex::env);
		api.cull(cullmode::back);
		ShowObject(tex::obj1pos,tex::obj1nrml,4);

	}

}
