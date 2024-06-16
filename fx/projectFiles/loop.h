namespace Loop
{

#include "camera.h"
#include "cubemap.h"
#include "object.h"

#include "..\\generated\apiReflection.h"


	bool isPrecalc = false;

	void Precalc()
	{
		gapi.setIA(topology::triList);
		ConstBuf::Update(5, ConstBuf::global);
		for (int i = 0; i < 6; i++) { ConstBuf::ConstToVertex(i); ConstBuf::ConstToPixel(i); }
		isPrecalc = true;
	}

	void frameConst()
	{
		ConstBuf::frame.time = (float)(timer::frameBeginTime * .01);
		ConstBuf::UpdateFrame();
	}

		
	void mainLoop()
	{
		cmdCounter = 0;

		if (!isPrecalc) Precalc();

		regCam();
		frameConst();

		api.IA(topology::triList);
		api.CalcCubemap( texture::env);
		api.CalcObject( texture::obj1pos,texture::obj1nrml);

		api.Blending( blendmode::off, blendop::add);
		api.RenderTarget(texture::mainRT, 0 );

		//setCam(0);
		float t = (timer::timeCursor / (SAMPLES_IN_FRAME * 60.f));
		slideCam(0, 1, t);

		api.Depth( depthmode::off);
		api.Clear( 0, 0, 0, 1 );

		//api.ShowCubemap( texture::env);

		api.Depth(depthmode::on);
		api.ClearDepth();
		api.Cull( cullmode::back);
		api.ShowObject( texture::obj1pos, texture::obj1nrml, 4, -500, 0, -500 );
		api.ShowObject(texture::obj1pos, texture::obj1nrml, 4, -500, 0, 500);
		api.ShowObject(texture::obj1pos, texture::obj1nrml, 4, 500, 0, -500);
		api.ShowObject(texture::obj1pos, texture::obj1nrml, 4, 500, 0, 500);

		
		playTrack();
	}

}
