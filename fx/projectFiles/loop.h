
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
		gfx::SetInputAsm(topology::lineList);
		ConstBuf::Update(ConstBuf::cBuffer::global);
		for (int i = 0; i < 6; i++) { ConstBuf::ConstToVertex((ConstBuf::cBuffer)i); ConstBuf::ConstToPixel((ConstBuf::cBuffer)i); }
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

	API(cameraMan)
	{
		BasicCam::setCamKey(0, keyType::set, 3021, 22, 46, 2, 51, 0, 0, 508, 77, 65, sliderType::follow, 0, 0, -1, camAxis::local, 0, 3, 0, 0);
		BasicCam::processCam();
	}

#define CALLER ,const std::source_location caller = std::source_location::current()
#define reflect editor::paramEdit::reflect_f(&in, caller, std::source_location::current())
#define reflect_close cmdLevel--

#define api2(name, ...) struct CAT(name,_params) {__VA_ARGS__};\
	void name(CAT(name,_params) in CALLER)

	namespace object1 {

		#pragma pack(push, 1)
		struct params {
			int x; 
			int y;
			int z;
			texture target;
		};
		#pragma pack(pop)

		void set(params in CALLER)
		{
		reflect;

			int c = 0;

		reflect_close;
		};

	
	}

	
	api2(func2, int x; int y; int z=10;)
	{
		reflect;
		int a = 0;
		reflect_close;
	}



	void mainLoop()
	{
		BasicCam::camPass = false;
		BasicCam::camCounter = 0;

		if (!isPrecalc)
		{
			Precalc();
		}

		cmdCounter = precalcOfs;

		frameConst();

		int b = 22;

		object1::set({
            	.x = 8,
            	.y = 0,
            	.z = b,
            	.target = texture::mainRT
            }); 
		
		gfx::SetInputAsm(topology::triList);
		gfx::SetBlendMode(blendmode::off, blendop::add);

		Cubemap::CalcCubemap(texture::env);
		Object::CalcObject(texture::obj1pos, texture::obj1nrml);

		gfx::SetRT(texture::mainRT, 0);
		gfx::ClearRT(255, 255, 255, 255);

		cameraMan();

		Cubemap::ShowCubemap(texture::env);

		gfx::SetDepthMode(depthmode::on);
		gfx::ClearRTDepth();
		gfx::SetCull(cullmode::back);
		gfx::SetBlendMode(blendmode::off, blendop::add);
		
		Object::ShowObject(texture::obj1pos, texture::obj1nrml, 0, 120, 82, 112);
		
		

		//gfx::SetRT(texture::mainRT, 0);
		//gfx::ClearRT(0, 0, 0, 255);
		//tracker::Track(120);

		//gApi.ClearRT(0, 0, 0, 0);

	}

}
