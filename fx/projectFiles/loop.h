
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
				.angle = 67,
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

	void Compose()
	{
		RenderTarget::Set({texture::mainRT,0});

		BlendMode::Set({
			.mode = blendmode::off,
			.op = blendop::add
		});

		ps::output.textures.screen = texture::pBuf;
		ps::output.textures.screenMid = texture::pBufMid;
		ps::output.textures.screenLow = texture::pBufLow;
		ps::output.samplers = {
				.sam1Filter = filter::linear,
				.sam1AddressU = addr::wrap,
				.sam1AddressV = addr::wrap
		};

		vs::quad.set();
		ps::output.set();

		Drawer::NullDrawer({1,1});
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

		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
		});

		DepthBuf::Mode({ depthmode::off });

		Culling::Set({cullmode::back});
		
		cameraMan::run({});

		//Object::Saggitarius({.quality = 1});
		//Object::Zenith({.quality = 1});
		//Object::Fish({.quality = 1});
		//Object::Libra({.quality = 1});
		//Object::Aquarius({.quality = 1});
		//Object::Crab({ .quality = 1 });
		//Object::Twins({ .quality = 1 });
		//Object::LeoBigStar({ .quality = 1 });
		//Object::Capri({ .quality = 1 });
		//Object::Taurus({ .quality = 1 });
		//Object::Scorpio({ .quality = 1 });
		//Object::Aries({ .quality = 1 });
		Object::Virgo({ .quality = 1 });

		Compose();

	}


	void track__()
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
		tracker::Track({});
			
	}


	void mainLoop()
	{
		
		scene1();
		//track__();

	}

}
