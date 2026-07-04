
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
				.camType = keyType::slide,
				.eye_x = 9930,
				.eye_y = 222,
				.eye_z = 71222,
				.at_x = 756,
				.at_y = 0,
				.at_z = 0,
				.up_x = 0,
				.up_y = 254,
				.up_z = -17,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = 0,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});

			BasicCam::setCamKey({
				.camTime = 615,
				.camType = keyType::slide,
				.eye_x = -5610,
				.eye_y = -1158,
				.eye_z = -22856,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -7,
				.up_y = 253,
				.up_z = 24,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = -3,
				.slide_y = 0,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = 0,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});

			BasicCam::processCam();
			
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
		
		//cameraMan::run({});

		int Dur = 20;
		int t = timer::timeCursor / SAMPLES_IN_FRAME /FRAMES_PER_SECOND/Dur;

		switch (t)
		{
		case 0:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 57,
				.eye_y = 1236,
				.eye_z = -1975,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -3,
				.up_y = 216,
				.up_z = 135,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -110,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
				});
			BasicCam::processCam();


			Object::Zenith({.quality = 1});
			break;
		case 1:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = -1086,
				.eye_y = -1198,
				.eye_z = -499,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -163,
				.up_y = 180,
				.up_z = -75,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 70,
				.slide_z = -60,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Saggitarius({ .quality = 1 });
			break;
		case 2:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 4,
				.eye_y = 3800,
				.eye_z = 79,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 14,
				.up_y = -5,
				.up_z = 254,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 310,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();

			Object::Crab({ .quality = 1 });
			break;
		case 3:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = -1926,
				.eye_y = -1960,
				.eye_z = 3342,
				.at_x = 0,
				.at_y = 115,
				.at_z = 0,
				.up_x = -29,
				.up_y = 248,
				.up_z = 51,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 40,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 1,
				.fly_z = -2,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Aries({ .quality = 1 });
			break;
		case 4:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 2360,
				.eye_y = -2052,
				.eye_z = -402,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 163,
				.up_y = 193,
				.up_z = -27,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -211,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Scorpio({ .quality = 1 });
			break;
		case 5:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 1015,
				.eye_y = -2195,
				.eye_z = 1015,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 150,
				.up_y = 139,
				.up_z = 150,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 250,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::LeoBigStar({ .quality = 1 });
			break;
		case 6:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = -2462,
				.eye_y = -4185,
				.eye_z = 762,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -207,
				.up_y = 133,
				.up_z = 64,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -120,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Virgo({ .quality = 1 });
			break;
		case 7:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 644,
				.eye_y = 1711,
				.eye_z = 940,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 119,
				.up_y = -141,
				.up_z = 175,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -50,
				.axisType = camAxis::global,
				.fly_x = 3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Capri({ .quality = 1 });
			break;
		case 8:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 57,
				.eye_y = -136,
				.eye_z = -1975,
				.at_x = 0,
				.at_y = 222,
				.at_z = 0,
				.up_x = -3,
				.up_y = 216,
				.up_z = 135,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -416,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
				});
			BasicCam::processCam();
			Object::Taurus({ .quality = 1 });
			//
			break;
		case 9:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 22,
				.eye_y = -958,
				.eye_z = -2169,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 1,
				.up_y = 233,
				.up_z = -102,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Libra({ .quality = 1 });
			break;
		case 10:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 622,
				.eye_y = -1022,
				.eye_z = -1822,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 38,
				.up_y = 225,
				.up_z = -113,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -40,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Aquarius({ .quality = 1 });
			break;
		case 11:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = -1357,
				.eye_y = 1027,
				.eye_z = -2163,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -50,
				.up_y = -236,
				.up_z = -80,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = 3,
				.fly_y = 0,
				.fly_z = 10,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Fish({ .quality = 1 });
			break;
		case 12:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 57,
				.eye_y = 1236,
				.eye_z = -1975,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -3,
				.up_y = 216,
				.up_z = 135,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 10,
				.jitter = 0
				});
			BasicCam::processCam();
			Object::Twins({ .quality = 1 });
			break;
		}


		Compose();

	}

	void scene2()
	{
		BasicCam::camPass = false;
		BasicCam::camCounter = 0;

		if (!isPrecalc)
		{
			Precalc();
		}

		cmdCounter = precalcOfs;

		frameConst();

		InputAsm::Set({ topology::triList });

		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});


		DepthBuf::Mode({ depthmode::off });

		Culling::Set({ cullmode::back });

		//cameraMan::run({});

		int Dur = 20;
		int t = timer::timeCursor / SAMPLES_IN_FRAME / FRAMES_PER_SECOND / Dur;

		switch (t)
		{
		case 0:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 3,
				.eye_y = -1096,
				.eye_z = 2,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -193,
				.up_y = 0,
				.up_z = -165,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 10,
				.slide_z = -280,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 2,
				.jitter = 0
			});
			BasicCam::processCam();


			Object::Zenith({.quality = 1});
			break;
		case 1:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 26,
				.eye_y = 3002,
				.eye_z = 41,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 138,
				.up_y = 0,
				.up_z = 214,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 160,
				.axisType = camAxis::global,
				.fly_x = 0,
				.fly_y = -3,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Saggitarius({ .quality = 1 });
			break;
		case 2:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 4,
				.eye_y = 3800,
				.eye_z = 79,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 14,
				.up_y = -5,
				.up_z = 254,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 437,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();

			Object::Crab({ .quality = 1 });
			break;
		case 3:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = -170,
				.eye_y = 3689,
				.eye_z = -259,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 139,
				.up_y = 21,
				.up_z = 212,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 440,
				.axisType = camAxis::global,
				.fly_x = 0,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Aries({ .quality = 1 });
			break;
		case 4:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 526,
				.eye_y = -928,
				.eye_z = 286,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -188,
				.up_y = -138,
				.up_z = -102,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -511,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Scorpio({ .quality = 1 });
			break;
		case 5:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t * SAMPLES_IN_FRAME * FRAMES_PER_SECOND * Dur,
				.camType = keyType::set,
				.eye_x = 1015,
				.eye_y = -2195,
				.eye_z = 1015,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 150,
				.up_y = 139,
				.up_z = 150,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -450,
				.axisType = camAxis::global,
				.fly_x = -13,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
				});
			BasicCam::processCam();
			Object::LeoBigStar({ .quality = 1 });
			break;
		case 6:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 0,
				.eye_y = 0,
				.eye_z = -11101,
				.at_x = 0,
				.at_y = 0,
				.at_z = 11110,
				.up_x = 244,
				.up_y = -71,
				.up_z = -5,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 2110,
				.axisType = camAxis::global,
				.fly_x = 0,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Virgo({ .quality = 1 });
			break;
		case 7:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 49,
				.eye_y = 1120,
				.eye_z = 12,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -246,
				.up_y = 11,
				.up_z = -64,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 22,
				.slide_y = 10,
				.slide_z = -10,
				.axisType = camAxis::local,
				.fly_x = 3,
				.fly_y = -2,
				.fly_z = -3,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Capri({ .quality = 1 });
			break;
		case 8:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 49,
				.eye_y = 1120,
				.eye_z = 12,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -246,
				.up_y = 11,
				.up_z = -64,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -416,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Taurus({ .quality = 1 });
			//
			break;
		case 9:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 134,
				.eye_y = 1624,
				.eye_z = 67,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 227,
				.up_y = -23,
				.up_z = 113,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = -410,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 3,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Libra({ .quality = 1 });
			break;
		case 10:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 239,
				.eye_y = -2499,
				.eye_z = 277,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 164,
				.up_y = 37,
				.up_z = 190,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -140,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Aquarius({ .quality = 1 });
			break;
		case 11:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = -8,
				.eye_y = 5,
				.eye_z = -1009,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 0,
				.up_y = 114,
				.up_z = 0,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = 1,
				.fly_y = 2,
				.fly_z = 3,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Fish({ .quality = 1 });
			break;
		case 12:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 10,
				.eye_y = 5111,
				.eye_z = 0,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -254,
				.up_y = 0,
				.up_z = -21,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 710,
				.axisType = camAxis::global,
				.fly_x = 0,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Twins({ .quality = 1 });
			break;
		}


		Compose();

	}

	struct hero_ {
		float4 pos = { 0,0,0,0 };
		float4 normal = { 0,0,0,0 };
		float axisAngle = 0;
		float axisAngleSpeed = 0;
		float axisAngleAccel = 0.01;
		float maxAxisAngleSpeed = .1;
		float yOffset = 40;
		bool gravityMode = true;
		float speed = 0;
		float accel = 0.01;
		float maxSpeed = .3;
		float autoBrake = .9;
		int lineIndex = 0;
		float pointIndex = 0;
		float angle = 100;
		float jumpHeight = 0;
	};

	hero_ hero;

	float distance(const float4& p1, const float4& p2) {
		float dx = p2.x - p1.x;
		float dy = p2.y - p1.y;
		float dz = p2.z - p1.z;

		return sqrt(dx * dx + dy * dy + dz * dz);
	}

	float4 lerp3(const float4& a, const float4& b, float t) {
		return float4{
			a.x + t * (b.x - a.x),
			a.y + t * (b.y - a.y),
			a.z + t * (b.z - a.z),
			a.w // Сохраняем оригинальное значение w из первой точки
		};
	}

	float frac(float x) {
		return x - floor(x);
	}

	float4 normalize(const float4& v) {
		// Считаем длину вектора по формуле Пифагора
		float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

		// Защита от деления на ноль (если вектор нулевой)
		if (length < 0.00001f) {
			return float4{ 0.0f, 0.0f, 0.0f, v.w };
		}

		// Возвращаем нормализованный вектор
		return float4{
			v.x / length,
			v.y / length,
			v.z / length,
			v.w // Поле w оставляем оригинальным
		};
	}

	float4 cross(const float4& a, const float4& b) {
		return float4{
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x,
			0.0f // Для векторов направления w обычно равен 0
		};
	}

	float4 calculate_camera_up(const float4& eye, const float4& at) {
		// 1. Вычисляем нормализованный вектор взгляда (Forward)
		float4 forward = { at.x - eye.x, at.y - eye.y, at.z - eye.z, 0.0f };
		forward = normalize(forward);

		// 2. Задаем базовый мировой вектор "верх" (обычно ось Y)
		float4 world_up = { 0.0f, 1.0f, 0.0f, 0.0f };

		// КРАЙНИЙ СЛУЧАЙ (Gimbal Lock): Если камера смотрит строго вверх или строго вниз,
		// вектор forward станет коллинеарен world_up. Векторное произведение выдаст 0.
		// В этом случае временно меняем мировой ориентир на ось Z.
		float dot = forward.x * world_up.x + forward.y * world_up.y + forward.z * world_up.z;
		if (std::abs(dot) > 0.999f) {
			world_up = float4{ 0.0f, 0.0f, 1.0f, 0.0f }; // Если смотрим вертикально, "верх" берем от Z
		}

		// 3. Находим вектор "право" (Right) через Cross Product
		float4 right = normalize(cross(forward, world_up));

		// 4. Находим финальный "верх" (Up), перпендикулярный и взгляду, и правому вектору
		float4 up = normalize(cross(right, forward));

		return up;
	}

	// 2. ФУНКЦИЯ ПОЛУЧЕНИЯ НАПРАВЛЕНИЯ МЕЖДУ ДВУМЯ ТОЧКАМИ
	// Возвращает единичный вектор направления от точки 'from' к точке 'to'
	float4 direction_between(const float4& from, const float4& to) {
		// Находим вектор разности (вектор направления)
		float4 dir = {
			to.x - from.x,
			to.y - from.y,
			to.z - from.z,
			from.w // w-компоненту обычно наследуют от базовой точки
		};

		// Вызываем отдельную функцию нормализации
		return normalize(dir);
	}

	void processGravity()
	{
		if (!hero.gravityMode) return;

		float minDistance = 1e9f; // Инициализируем заведомо большим числом
		float4 closestPoint = { 0.0f, 0.0f, 0.0f, 0.0f };
		bool foundPoint = false;

		//Находим ОДНУ самую ближайшую точку
		for (int i = 0; i < Object::starLineList.lineCount; i++)
		{
			for (int j = 0; j < Object::starLineList.line[i].pointCount; j++)
			{
				float dst = distance(hero.pos, Object::starLineList.line[i].point[j]);

				// Если эта точка ближе, чем все предыдущие, запоминаем её
				if (dst < minDistance)
				{
					minDistance = dst;
					closestPoint = Object::starLineList.line[i].point[j];
					foundPoint = true;
					hero.lineIndex = i;
					hero.pointIndex = (float)j;
				}
			}
		}

		hero.pos = lerp3(hero.pos, closestPoint, .1);

		if (minDistance < .1) hero.gravityMode = false;
	}

	void Respawn()
	{
		if (GetAsyncKeyState('R'))
		{
			int range = 5;
			hero.pos.x = rand() % range - range / 2;
			hero.pos.y = rand() % range - range / 2;
			hero.pos.z = rand() % range - range / 2;
			int startLine = 0;
			int startPoint = 0;
			float4 destPoint = Object::starLineList.line[startLine].point[startPoint];
			hero.pos.x += destPoint.x;
			hero.pos.y += destPoint.y;
			hero.pos.z += destPoint.z;
			hero.gravityMode = true;
		}
	}

	bool gameCam = true;
	XMVECTOR tUP = { 0,1,0 };
	XMVECTOR tFORWARD = { 0,0,1 };
	float4 follow = { 0,0,0,0 };

	void scene3()
	{
		BasicCam::camPass = false;
		BasicCam::camCounter = 0;

		if (!isPrecalc)
		{
			Precalc();
		}

		cmdCounter = precalcOfs;

		frameConst();

		InputAsm::Set({ topology::triList });

		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});


		DepthBuf::Mode({ depthmode::off });

		Culling::Set({ cullmode::back });

		//cameraMan::run({});

		int Dur = 20;
		int t = timer::timeCursor / SAMPLES_IN_FRAME / FRAMES_PER_SECOND / Dur;
				
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = 0*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 43,
				.eye_y = -52,
				.eye_z = 1000,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 0,
				.up_y = 100,
				.up_z = 0,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = -18,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});

			//BasicCam::processCam();
			
			RenderTarget::Set({ texture::pBuf,0 });
			RenderTarget::Clear({ 0,0,0,0 });
			DepthBuf::Clear({});

			RenderTarget::Set({ texture::pBufMid,0 });
			RenderTarget::Clear({ 0,0,0,0 });

			RenderTarget::Set({ texture::pBufLow,0 });
			RenderTarget::Clear({ 0,0,0,0 });

			
			

			RenderTarget::Set({ texture::pBuf,0 });
			//

			Object::initPatches();

			Respawn();
			processGravity();


			if (GetActiveWindow() == hWnd)
			{
				if (GetAsyncKeyState(VK_SPACE))
				{
					hero.jumpHeight += .05;
					auto d = tUP + tFORWARD * hero.speed;
					hero.pos.x += XMVectorGetX(d);
					hero.pos.y += XMVectorGetY(d);
					hero.pos.z += XMVectorGetZ(d);
			
				}

				if (GetAsyncKeyState('D'))
				{
					hero.axisAngleSpeed -= hero.axisAngleAccel;
				}

				if (GetAsyncKeyState('A'))
				{
					hero.axisAngleSpeed += hero.axisAngleAccel;
				}
			}

			if (hero.jumpHeight > 1)
			{
				hero.gravityMode = true;
			}
			hero.jumpHeight *= .99;


			hero.axisAngleSpeed *= hero.autoBrake;
			hero.axisAngleSpeed = clamp(hero.axisAngleSpeed, -hero.maxAxisAngleSpeed, hero.maxAxisAngleSpeed);
			hero.axisAngle += hero.axisAngleSpeed;

			if (GetActiveWindow() == hWnd)
			{
				if (GetAsyncKeyState('W'))
				{
					hero.speed += hero.accel;
				}
				if (GetAsyncKeyState('S'))
				{
					hero.speed -= hero.accel;
				}
			}

			hero.speed *= hero.autoBrake;
			hero.speed = clamp(hero.speed, -hero.maxSpeed, hero.maxSpeed);
			
			hero.pointIndex += hero.speed;
			hero.pointIndex = clamp(hero.pointIndex, 1., (float)(Object::starLineList.line[hero.lineIndex].pointCount - 1));

			float t0 = floor(hero.pointIndex);
			float t1 = frac(hero.pointIndex);

			float4 current_node = Object::starLineList.line[hero.lineIndex].point[(int)t0];
			float4 next_node = Object::starLineList.line[hero.lineIndex].point[(int)t0+1];
			//движение по пути
			float4 follow_node = Object::starLineList.line[hero.lineIndex].point[(int)t0-1];
			
			if (!hero.gravityMode)
			{
				hero.pos = lerp3(current_node, next_node, t1);
				follow = lerp3(follow_node, current_node, t1);
			}

			


			//float4 forward = direction_between(current_node, next_node);
			//float4 base_up = calculate_camera_up(current_node, next_node);
			//float4 right = normalize(cross(forward, base_up));
			///float4 up = normalize(cross(right, forward));

			if (!hero.gravityMode)
			{
				float cd = 5.0f; // Дистанция до героя

				// --- ПЕРЕМЕННЫЕ ДЛЯ ЗАПАЗДЫВАНИЯ КАМЕРЫ И МЫШИ (static) ---
				static XMVECTOR lastUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				static XMVECTOR smoothedLineEye = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
				static XMVECTOR smoothedLineUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				static XMVECTOR smoothedPlayerUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

				static float mouseYaw = 0.0f;
				static float mousePitch = 0.0f;
				static bool isMouseInitialized = false;

				// --- НАСТРОЙКИ СГЛАЖИВАНИЯ И ЧУВСТВИТЕЛЬНОСТИ МЫШИ ---
				float deltaTime = 0.016f;        // Реальный dt кадра
				float distanceLerpFactor = 7.0f;
				float trackAngularLerpFactor = 1.0f;
				float playerAngularLerpFactor = 12.0f;

				float mouseSensitivity = 0.002f; // Чувствительность мыши (в радианах на пиксель)

				// ==========================================
				// ЭТАП 0: ЗАХВАТ И РАСЧЕТ ДЕЛЬТ МЫШИ (ЧЕРЕЗ WIN32)
				// ==========================================
				// Получаем хэндл текущего окна игры (замените dx11::hwnd на вашу переменную окна)
				HWND hwnd = hWnd;


				if (GetActiveWindow() == hwnd && gameCam) // Считаем мышь, только если окно в фокусе
				{
					RECT rect;
					GetWindowRect(hwnd, &rect);

					// Находим центр окна
					int centerX = rect.left + (rect.right - rect.left) / 2;
					int centerY = rect.top + (rect.bottom - rect.top) / 2;

					POINT currentPos;
					if (GetCursorPos(&currentPos))
					{
						if (!isMouseInitialized)
						{
							// В самый первый кадр просто сбрасываем мышь в центр без расчета дельт,
							// иначе камеру мгновенно улетит в сторону из-за старых координат курсора.
							SetCursorPos(centerX, centerY);
							isMouseInitialized = true;
						}
						else
						{
							// Считаем разницу между центром и текущим положением
							int deltaX = currentPos.x - centerX;
							int deltaY = currentPos.y - centerY;

							// Накапливаем углы (инвертируем Y, если нужно классическое управление)
							mouseYaw += (float)deltaX * mouseSensitivity;
							mousePitch += (float)deltaY * mouseSensitivity; // Минус, чтобы мышь вверх поднимала взгляд

							// Ограничиваем Pitch (взгляд вверх/вниз), чтобы камеру не вывернуло наизнанку (ограничение ~85 градусов)
							float pitchLimit = 1.48f / 2.; // В радианах (85 градусов)
							mousePitch = clamp(mousePitch, -pitchLimit, pitchLimit);

							// Возвращаем курсор обратно в центр для следующего кадра
							SetCursorPos(centerX, centerY);
						}
					}
				}
				else
				{
					// Если окно потеряло фокус (например, Alt+Tab), сбрасываем инициализацию
					isMouseInitialized = false;
				}

				// Расчет весов интерполяции для каждого компонента
				float t_dist = 1.0f - std::exp(-distanceLerpFactor * deltaTime);
				float t_track = 1.0f - std::exp(-trackAngularLerpFactor * deltaTime);
				float t_play = 1.0f - std::exp(-playerAngularLerpFactor * deltaTime);

				// -------------------------------------------------------------

				XMVECTOR p0 = XMVectorSet(follow.x, follow.y, follow.z, 0.0f);
				XMVECTOR p1 = XMVectorSet(hero.pos.x, hero.pos.y, hero.pos.z, 0.0f);

				XMVECTOR forward = XMVector3Normalize(p1 - p0);

				// ==========================================
				// ЭТАП 1: ПАРАЛЛЕЛЬНЫЙ ПЕРЕНОС ФРЕЙМА НИТИ
				// ==========================================
				XMVECTOR dotResult = XMVector3Dot(lastUp, forward);
				XMVECTOR upOnForward = XMVectorMultiply(forward, dotResult);
				XMVECTOR lineUp = XMVector3Normalize(XMVectorSubtract(lastUp, upOnForward));

				if (XMVector3Less(XMVector3LengthEst(lineUp), XMVectorSet(0.001f, 0.001f, 0.001f, 0.001f))) {
					XMVECTOR alternative = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
					XMVECTOR right = XMVector3Normalize(XMVector3Cross(alternative, forward));
					lineUp = XMVector3Normalize(XMVector3Cross(forward, right));
				}
				lastUp = lineUp;

				// ==========================================
				// ЭТАП 2: ВРАЩЕНИЕ ВОКРУГ ОСИ НИТИ (МГНОВЕННЫЙ ЦЕЛЕВОЙ UP ГЕРОЯ)
				// ==========================================
				XMVECTOR rotationQuat = XMQuaternionRotationAxis(forward, hero.axisAngle);
				XMVECTOR targetUp = XMVector3Normalize(XMVector3Rotate(lineUp, rotationQuat));

				// ==========================================
				// ЭТАП 3: МАТРИЦА ГЕРОЯ (МГНОВЕННАЯ, COLUMN-MAJOR)
				// ==========================================
				XMVECTOR HeroRight = XMVector3Normalize(XMVector3Cross(targetUp, forward));
				XMVECTOR HeroRealUp = XMVector3Normalize(XMVector3Cross(forward, HeroRight));
				XMVECTOR HeroPosition = XMVectorSet(0, 0, 0, 1.0f);
				tUP = HeroRealUp;
				tFORWARD = forward;

				Object::heroWorld = XMMatrixTranspose(XMMATRIX(
					XMVectorSet(XMVectorGetX(HeroRight), XMVectorGetX(HeroRealUp), XMVectorGetX(forward), 0.0f),
					XMVectorSet(XMVectorGetY(HeroRight), XMVectorGetY(HeroRealUp), XMVectorGetY(forward), 0.0f),
					XMVectorSet(XMVectorGetZ(HeroRight), XMVectorGetZ(HeroRealUp), XMVectorGetZ(forward), 0.0f),
					XMVectorSet(XMVectorGetX(HeroPosition), XMVectorGetY(HeroPosition), XMVectorGetZ(HeroPosition), 1.0f)
				));

				// ==========================================
				// ЭТАП 4: СГЛАЖИВАНИЕ ДВИЖЕНИЯ ВДОЛЬ ОСИ НИТИ
				// ==========================================
				XMVECTOR targetLineEye = p1 - forward * cd;
				smoothedLineEye = XMVectorLerp(smoothedLineEye, targetLineEye, t_dist);

				// ==========================================
				// ЭТАП 5: РАЗДЕЛЬНОЕ СГЛАЖИВАНИЕ ОРИЕНТАЦИЙ
				// ==========================================
				smoothedLineUp = XMVector3Normalize(XMVectorLerp(smoothedLineUp, lineUp, t_track));
				XMVECTOR targetPlayerUp = XMVector3Normalize(XMVector3Rotate(smoothedLineUp, rotationQuat));
				smoothedPlayerUp = XMVector3Normalize(XMVectorLerp(smoothedPlayerUp, targetPlayerUp, t_play));

				XMVECTOR currentCameraUp = smoothedPlayerUp;

				// ==========================================
				// ЭТАП 6: РАСЧЕТ ИНТЕНСИВНОСТИ ОСМОТРА МЫШЬЮ ОТ СКОРОСТИ
				// ==========================================
				float currentSpeedAbs = std::abs(hero.speed);
				float maxSpeed = max(hero.maxSpeed, 0.0001f);

				float mouseInfluenceFactor = 1.0f - (currentSpeedAbs / maxSpeed);
				mouseInfluenceFactor = clamp(mouseInfluenceFactor, 0.0f, 1.0f);

				float finalYaw = mouseYaw * mouseInfluenceFactor;
				float finalPitch = mousePitch * mouseInfluenceFactor;

				// ==========================================
	// ЭТАП 7: ФОРМИРОВАНИЕ ТОЧЕК КАМЕРЫ ЧЕРЕЗ ЗАНИЖЕННЫЙ ЦЕНТР ВРАЩЕНИЯ (PIVOT)
	// ==========================================

	// 1. Создаем виртуальную точку опоры (Pivot). 
	// Мы опускаем центр вращения камеры НИЖЕ ног персонажа по вектору currentCameraUp.
	// За счет этого персонаж визуально поднимется вверх и окажется ровно на нижней трети экрана!
				float pivotOffset = -3.3f; // Подберите это значение: больше число = персонаж ниже на экране
				XMVECTOR cameraPivot = p1 - currentCameraUp * pivotOffset;

				// 2. Находим базовую позицию камеры (без мыши) относительно этой точки опоры
				XMVECTOR baseCameraEye = smoothedLineEye + currentCameraUp * 2.0f;

				// 3. Вычисляем базовый вектор смещения от заниженного центра опоры
				XMVECTOR baseOffsetFromPivot = XMVectorSubtract(baseCameraEye, cameraPivot);

				// 4. Строим локальные оси камеры для правильного наложения мыши
				XMVECTOR camForward = XMVector3Normalize(XMVectorNegate(baseOffsetFromPivot)); // Направление строго на Pivot
				XMVECTOR camRight = XMVector3Normalize(XMVector3Cross(currentCameraUp, camForward));
				XMVECTOR camUp = XMVector3Normalize(XMVector3Cross(camForward, camRight));

				// 5. Создаем кватернионы вращения мыши вокруг локальных осей
				XMVECTOR mouseYawQuat = XMQuaternionRotationAxis(camUp, finalYaw);
				XMVECTOR mousePitchQuat = XMQuaternionRotationAxis(camRight, finalPitch);
				XMVECTOR mouseCombinedQuat = XMQuaternionMultiply(mousePitchQuat, mouseYawQuat);

				// 6. Вращаем вектор смещения вокруг нашей виртуальной точки опоры
				XMVECTOR rotatedOffsetFromPivot = XMVector3Rotate(baseOffsetFromPivot, mouseCombinedQuat);

				// 7. ФИНАЛЬНАЯ ПОЗИЦИЯ КАМЕРЫ НА СФЕРЕ ОБЛЕТА
				XMVECTOR finalCameraEye = XMVectorAdd(cameraPivot, rotatedOffsetFromPivot);

				// 8. ТОЧКА НАЗНАЧЕНИЯ (Куда смотрим)
				// Камера всегда жестко и стабильно смотрит в центр вращения cameraPivot.
				// Никаких дополнительных кватернионов наклона взгляда. Математика чистая и без прыжков!
				XMVECTOR finalCameraAt = cameraPivot;

				// ==========================================
				// ЭТАП 8: СБОРКА МАТРИЦЫ ВИДА
				// ==========================================
				ConstBuf::camera = {
					.world = XMMatrixIdentity(),
					.view = XMMatrixTranspose(XMMatrixLookAtLH(finalCameraEye, finalCameraAt, currentCameraUp)),
					.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(DegreesToRadians(hero.angle), dx11::iaspect, 0.01f, 100.0f))
				};

				ConstBuf::Update(ConstBuf::cBuffer::camera);
				ConstBuf::Set(ConstBuf::cBuffer::camera, ConstBuf::target::both);
			}
			//


			//
			Object::HeroMesh.Load("..//fx//projectFiles//hero.obj");
			Object::MeshPtr = &Object::HeroMesh;
			Object::Mesh({
					.quality = 1,
					.xPos = (int)(hero.pos.x*100),
					.yPos = (int)(hero.pos.y*100),
					.zPos = (int)(hero.pos.z*100),
					.brightness = 14,
					.tickness = 2,
					.stencil = switcher::on,
					.zoom = -81
				});

			//Object::BossMesh.Load("..//fx//projectFiles//edged.obj");
			/*Object::MeshPtr = &Object::BossMesh;
			Object::Mesh({
				.quality = 1,
				.xPos = 0,
				.yPos = 9,
				.zPos = 0,
				.brightness = 114,
				.tickness = 2,
				.stencil = switcher::on,
				.zoom = 110
				});*/
				

			Object::Girl({
					.quality = 1,
					.xPos = 0,
					.yPos = 0,
					.zPos = 0,
					.brightness = 19,
					.tickness = 2,
					.stencil = switcher::on
				});



		Compose();

	}

	void scene4()
	{
		BasicCam::camPass = false;
		BasicCam::camCounter = 0;

		if (!isPrecalc)
		{
			Precalc();
		}

		cmdCounter = precalcOfs;

		frameConst();

		InputAsm::Set({ topology::triList });

		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});


		DepthBuf::Mode({ depthmode::off });

		Culling::Set({ cullmode::back });

		//cameraMan::run({});

		int Dur = 20;
		int t = timer::timeCursor / SAMPLES_IN_FRAME / FRAMES_PER_SECOND / Dur;

		
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t * SAMPLES_IN_FRAME * FRAMES_PER_SECOND * Dur,
				.camType = keyType::set,
				.eye_x = 10,
				.eye_y = 5111,
				.eye_z = 0,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -254,
				.up_y = 0,
				.up_z = -21,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 710,
				.axisType = camAxis::global,
				.fly_x = 0,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
				});
			BasicCam::processCam();
			Object::Twins({ .quality = 1 });
			//break;
		


		Compose();

	}

	void ProcessTrack()
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
		tracker::Music();
			
	}


	void mainLoop()
	{
		
		scene3();
		//track__();

	}

}
