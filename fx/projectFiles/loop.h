
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
			BasicCam::setCamKey({ 0, keyType::set, 3021, 22, 46, 2, 51, 0, 0, 508, 77, 65, sliderType::follow, 0, 0, -1, camAxis::local, 0, 3, 0, 0 });
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
		Culling::Set({ cullmode::back });
		BlendMode::Set({
			.mode = blendmode::off,
			.op = blendop::add
		});

		Object::Show({
			.geometry = texture::obj1pos,
			.normals = texture::obj1nrml,
			.quality = 0,
			.pos_x = 46,
			.pos_y = 56,
			.pos_z = 24
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
