
#include "graphicsAPI.h"

#include "camera.h"
#include "cubemap.h"
#include "object.h"

#include "..\sound\tracker.h"

int precalcOfs = 0;


namespace Loop
{


	bool isPrecalc = false;

	void Precalc()
	{
		gApi.SetInputAsm(topology::triList);
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
		cmdCounter = 0;
		cmdLevel = 0;

		if (!isPrecalc)
			Precalc();

		cmdCounter = precalcOfs;

		regCam();
		frameConst();

		gApi.SetInputAsm(topology::triList);
		Cubemap::CalcCubemap(texture::env);
		Object::CalcObject(texture::obj1pos,texture::obj1nrml);

		gApi.SetBlendMode(blendmode::off,blendop::add);
		gApi.SetRT(texture::mainRT,0);
		gApi.SetCamera(0, 0, 1, 0, 0, 0, 0, 1,0, 45);
		gApi.SetDepthMode(depthmode::off);
		gApi.ClearRT(0, 111, 235, 233);

		Cubemap::ShowCubemap(texture::env);

		gApi.SetDepthMode(depthmode::on);
		gApi.ClearRTDepth();
		gApi.SetCull(cullmode::back);

		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 5, 500, 0, -500);
		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 5, -500, 0, 500);
		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 5, -500, 0, -500);
		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 5, 500, 498, 500);

		tracker::playTrack();

		paramsAreLoaded = true;
	}

}
