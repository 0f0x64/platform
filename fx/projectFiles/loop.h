
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

	COMMAND(cameraMan)
	{
		#include REFLECT(cameraMan)

		auto a = typeDesc;

		BasicCam::setCamKey(0, keyType::set, 1984, 2301, 3145, -24, 671, -88, -6, 252, -33, 60, sliderType::slide, 0, 0, 0, camAxis::local, 0, 0, 0, 312);
		//BasicCam::setCamKey(884, keyType::slide, -2451, 1346, 157, 0, 267, -265, 98, 234, -16, 80,0, 0, 0, 0, 0, 0, 0);
		//BasicCam::setCamKey(1965, keyType::slide, -157, 3304, -215, 0, 0, 0, -86, 11, 239, 60, 0, 0, 0, 0, 0, 0, 0);
		BasicCam::processCam();

		REFLECT_CLOSE;
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
		gApi.SetBlendMode(blendmode::off, blendop::add);

		Cubemap::CalcCubemap(texture::env);
		Object::CalcObject(texture::obj1pos, texture::obj1nrml);

		gApi.SetRT(texture::mainRT, 0);
		gApi.ClearRT(255, 255, 255, 255);

		cameraMan();

		Cubemap::ShowCubemap(texture::env);

		gApi.SetDepthMode(depthmode::on);
		gApi.ClearRTDepth();
		gApi.SetCull(cullmode::back);
		gApi.SetBlendMode(blendmode::off, blendop::add);

		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 1, 504, 264, -500);
		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 5, -323, 219, 682);
		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 8, -492, 229, -495);
		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 5, 513, 282, 534);

		tracker::playTrack();

		paramsAreLoaded = true;
	}

}
