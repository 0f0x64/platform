
#include "graphicsAPI.h"

#include "camera.h"
#include "cubemap.h"
#include "object.h"

#include "sound\tracker.h"

int precalcOfs = 0;


namespace Loop
{


	bool isPrecalc = false;

	void Precalc()
	{
		gApi.SetInputAsm(topology::lineList);
		ConstBuf::Update(5, ConstBuf::global);
		for (int i = 0; i < 6; i++) { ConstBuf::ConstToVertex(i); ConstBuf::ConstToPixel(i); }
		isPrecalc = true;
		precalcOfs = cmdCounter;
	}

	void frameConst()
	{
		ConstBuf::frame.time = XMFLOAT4{ (float)(timer::frameBeginTime * .01) ,0,0,0};
		ConstBuf::frame.aspect = XMFLOAT4{dx11::aspect,dx11::iaspect, 0, 0};
		ConstBuf::UpdateFrame();
	}


	void mainLoop()
	{

		BasicCam::camPass = false;
		cmdCounter = 0;
		cmdLevel = 0;

		if (!isPrecalc)
			Precalc();

		cmdCounter = precalcOfs;

		frameConst();

		gApi.SetInputAsm(topology::triList);
		Cubemap::CalcCubemap(texture::env);
		Object::CalcObject(texture::obj1pos, texture::obj1nrml);

		gApi.SetBlendMode(blendmode::off, blendop::add);
		gApi.SetRT(texture::mainRT, 0);

		BasicCam::setCamKey(0, keyType::slide, -577, 1660, 2057, 0, 267, -265, 30, 220, -124, 60);
		BasicCam::setCamKey(892, keyType::slide, -2514, 1346, 157, 0, 267, -265, 98, 234, -16, 60);
		BasicCam::setCamKey(1965, keyType::slide, -157, 3304, -215, 0, 0, 0, -86, 11, 239, 60);
		BasicCam::processCam();
		
		
		gApi.SetDepthMode(depthmode::off);
		gApi.ClearRT(0, 111, 235, 233);

		Cubemap::ShowCubemap(texture::env);

		gApi.SetDepthMode(depthmode::on);
		gApi.ClearRTDepth();
		gApi.SetCull(cullmode::back);




		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 5, 504, 0, -500);
		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 5, -496, 2, 500);
		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 5, -492, 360, -495);
		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 4, 511, 365, 542);

		tracker::playTrack();

		paramsAreLoaded = true;
	}

}
