
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
		ConstBuf::frame.time = XMFLOAT4{ (float)(timer::frameBeginTime * .01) ,(float)timer::timeCursor/SAMPLES_IN_FRAME,0,0};
		ConstBuf::frame.aspect = XMFLOAT4{dx11::aspect,dx11::iaspect, 0, 0};
		ConstBuf::UpdateFrame();
	}

	COMMAND(cameraMan)
	{
		#include REFLECT(cameraMan)

		BasicCam::setCamKey(0, keyType::set, -1212, -685, -1676, 4, 56, 0, -2, 233, -102, 60, sliderType::follow, 2, 3, 8, camAxis::local, 0, 50, 0, 0);
		BasicCam::processCam();

		REFLECT_CLOSE;
	}

	void mainLoop()
	{
		if (editor::codeRecompiled) {
			editor::codeRecompiled = false;
			isPrecalc = false;
			paramsAreLoaded = false;
		}

		BasicCam::camPass = false;
		cmdCounter = 0;
		cmdLevel = 0;
		BasicCam::camCounter = 0;

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
		gApi.SetCull(cullmode::off);
		gApi.SetBlendMode(blendmode::off, blendop::add);
		
		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 0, 0, 0, 0);

		tracker::playTrack();

		paramsAreLoaded = true;
	}

}
