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

		
#define ref __FILE__, __LINE__

	//#define ShowObject(posTexture, nrmlTexture, quality, x,y,z) api.ShowObject(__FILE__, __LINE__, posTexture, nrmlTexture, quality, position_{x,y,z}) 

	void mainLoop()
	{
		cmdCounter = 0;

		//CalcCubemap(tex::env);

		if (!isPrecalc) Precalc();



		regCam();
		frameConst();

		IA(topology::triList);

		CalcCubemap( texture::env);
		CalcObject( texture::obj1pos,texture::obj1nrml);

		Blending( blendmode::off, blendop::add);
		RenderTarget(texture::mainRT, 0 );

		//setCam(0);
		float t = (timer::timeCursor / (SAMPLES_IN_FRAME * 60.f));
		slideCam(0, 1, t);

		Depth( depthmode::on);
		Clear( 0.2f, 0.2f, 0.2f, 1.f );
		ClearDepth();

		ShowCubemap( texture::env);
		Cull( cullmode::back);
		ShowObject( texture::obj1pos, texture::obj1nrml, 4, 0, 0, 0 );
		ShowObject( texture::obj1pos, texture::obj1nrml, 4, 0, 1, 0 );
		//ShowObject(tex::obj1pos, tex::obj1nrml, 4, 0, 1, 0);


		playTrack();
	}

}
