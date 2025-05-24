
#include "gApi.h"
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
		InputAsm::Set({topology::triList});
		ConstBuf::Update(ConstBuf::cBuffer::global);

		for (int i = 1 ;i < 4; i++) { 
			ConstBuf::Set((ConstBuf::cBuffer)i,ConstBuf::target::both); 
		}

		isPrecalc = true;
		precalcOfs = cmdCounter;
	}

	void frameConst()
	{
		ConstBuf::frame = {
			.time = XMFLOAT4{ (float)(timer::frameBeginTime * .01) ,(float)timer::timeCursor / SAMPLES_IN_FRAME,0,0},
			.aspect = XMFLOAT4{dx11::aspect,dx11::iaspect, 0, 0}
		};

		ConstBuf::Update(ConstBuf::cBuffer::frame);
	}

	namespace cameraMan {

		cmd(run)
		{
			reflect;
			BasicCam::setCamKey({
				.camTime = 0,
				.camType = keyType::set,
				.eye_x = 3021,
				.eye_y = 22,
				.eye_z = 46,
				.at_x = 2,
				.at_y = 51,
				.at_z = 0,
				.up_x = 0,
				.up_y = 508,
				.up_z = 77,
				.angle = 65,
				.sType = sliderType::follow,
				.slide_x = 0, 
				.slide_y = 0, 
				.slide_z = -1,
				.axisType = camAxis::local,
				.fly_x = 0, 
				.fly_y = 3, 
				.fly_z = 0,
				.jitter = 0
				});

			BasicCam::processCam();
			reflect_close;
		}
	}

	void scene1()
	{
		BasicCam::camPass = false;
		BasicCam::camCounter = 0;

		if (!isPrecalc)
		{
			Precalc();
		}

		cmdCounter = precalcOfs;

		frameConst();

		InputAsm::Set({topology::triList});
		BlendMode::Set({ blendmode::off,blendop::add });

		Cubemap::Calc({ texture::env });
		Object::Calc({ texture::obj1pos,texture::obj1nrml });

		RenderTarget::Set({ texture::mainRT,0 });
		RenderTarget::Clear({ 255,255,255,255 });

		cameraMan::run({});

		Cubemap::Show({ texture::env });

		DepthBuf::Mode({ depthmode::on });
		DepthBuf::Clear({});
		Culling::Set({cullmode::back});
		BlendMode::Set({
			.mode = blendmode::off,
			.op = blendop::add
		});

		Object::Show({
			.geometry = texture::obj1pos,
			.normals = texture::obj1nrml,
			.quality = 0,
			.pos_x = -25,
			.pos_y = -31,
			.pos_z = 7
		});
	}


	void track()
	{
		if (!isPrecalc)
		{
			Precalc();
		}

		cmdCounter = precalcOfs;

		frameConst();
		
		RenderTarget::Set({texture::mainRT,0});
		RenderTarget::Clear({ 11, 11, 11, 255 });
		DepthBuf::Clear({});
		tracker::Track({ 120 });
			
	}


	void mainLoop()
	{
		
		scene1();
		//track();

	}

}
