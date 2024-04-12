
//because crinkler 
typedef unsigned long uint32;
typedef long int32;
static inline int32 log2(float x)
{
	uint32 ix = (uint32&)x;
	uint32 exp = (ix >> 23) & 0xFF;
	int32 log2 = int32(exp) - 127;

	return log2;
}

using namespace DirectX;

namespace dx
{

	#if DebugMode
		void Log(const char* message)
		{
			#if EditMode
				OutputDebugString(message);
			#else	
				MessageBox(hWnd, message, "", MB_OK);
			#endif	
		}

	#endif

	ID3D11Device* device = NULL;
	ID3D11DeviceContext* context = NULL;
	IDXGISwapChain* swapChain = NULL;
	
	int width;
	int height;
	float aspect;
	float iaspect;

	#include "constBuf.h"
	#include "camera.h"
	#include "blend.h"
	#include "rasterizer.h"
	#include "depth.h"
	#include "sampler.h"
	#include "textures.h"
	#include "device.h"
	#include "shaders.h"
	#include "draw.h"
	#include "IA.h"

	void Init()
	{
		Device::Init();
		Rasterizer::Init();
		Depth::Init();
		Blend::Init();
		ConstBuf::Init();
		Sampler::Init();
		Shaders::Init();
		Textures::Init();
	}

	struct {
		void rt(int i) { Textures::SetRT(i); }
		void mips() { Textures::CreateMipMap(); }
		void depth(int i) { Depth::Set(i); }
		void draw(int instances, int quadcount) { Draw::NullDrawer(instances, quadcount); }
		void cam() { Camera::Set(); }
		void clear(float r, float g, float b, float a) { Draw::Clear(r, g, b, a); }
		void clearDepth() { Draw::ClearDepth(); }
		void blend(int mode, int op) { Blend::Set(mode, op); }
	} api;

	namespace blendmode { enum { off, on, alpha }; }
	namespace blendop { enum { add, sub, revsub, min, max }; }
	namespace depthmode { enum { off, on, readonly, writeonly }; }
	namespace filter { enum { linear, point, minPoint_magLinear }; }
	namespace addr { enum { clamp, wrap }; }
	
	#undef Texture
	#define Texture(name,type,format,width,height,mip,depth) name,
	namespace tex {
		enum {
			#include "..\projectFiles\texList.h"	
		};
	};

}
