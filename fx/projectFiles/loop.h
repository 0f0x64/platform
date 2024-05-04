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

		
//#define CalcCubemap(...) CalcCubemap(__FILE__, __LINE__,__VA_ARGS__)

#define ref __FILE__, __LINE__

	void mainLoop()
	{
		cmdCounter = 0;

		//CalcCubemap(tex::env);

		if (!isPrecalc) Precalc();

		regCam();
		frameConst();

		api.IA(ref,topology::triList);

		api.CalcCubemap(ref, tex::env);
		api.CalcObject(ref, tex::obj1pos,tex::obj1nrml);

		api.Blending(ref, blendmode::off);
		api.RenderTarget(ref);

		//setCam(0);
		float t = (timer::timeCursor / (SAMPLES_IN_FRAME * 60.f));
		slideCam(0, 1, t);

		api.Depth(ref, depthmode::on);
		api.Clear(ref, 0.2f, 0.2f, 0.2f, 1.f);
		api.ClearDepth(ref);

		api.ShowCubemap(ref, tex::env);
		api.Cull(ref, cullmode::back);
		api.ShowObject(ref, tex::obj1pos, tex::obj1nrml, 4, position{ 0,0,0 });
		api.ShowObject(ref, tex::obj1pos, tex::obj1nrml, 4, position{ 0, 1, 0 });

		playTrack();
	}

}
