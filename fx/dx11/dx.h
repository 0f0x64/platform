typedef unsigned long uint32;
typedef long int32;
static inline int32 log2(float x)
{
	uint32 ix = (uint32&)x;
	uint32 exp = (ix >> 23) & 0xFF;
	int32 log2 = int32(exp) - 127;

	return log2;
}

namespace dx11
{

	#include <d3d11.h>
	#include <d3dcompiler.h>
	#include "DirectXMath.h"
	#include <DirectXPackedVector.h>

	using namespace DirectX;

	#if DebugMode | EditMode

		void Log(const char* message)
		{
			#if EditMode
				OutputDebugString(message);
			#else	
				MessageBox(hWnd, message, "", MB_OK);
			#endif	
		}

		#define LogIfError(text)  if (FAILED(hr)) { Log("CreateTexture2D error\n"); return; } 
		#define LogBlobIfError if (FAILED(hr)) { Log((char*)pErrorBlob->GetBufferPointer()); }

	#else
		
		#define LogIfError(text)  
		#define LogBlobIfError

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
		void setIA(int topology) { InputAssembler::IA(topology); }
		void rt(int i) { Textures::RenderTarget(i); }
		void mips() { Textures::CreateMipMap(); }
		void depth(int i) { Depth::Depth(i); }
		void draw(int quadcount, int instances = 1) { Draw::NullDrawer(quadcount, instances); }
		void cam(position_ eye, position_ at, position_ up, float angle) { Camera::Camera(eye, at, up, angle); }
		void clear(color4_ c) { Draw::Clear(c); }
		void clearDepth() { Draw::ClearDepth(); }
		void blend(int mode, int op = blendop::add) { Blend::Blending(mode, op); }
		void cull(int i) { Rasterizer::Cull(i); }
		void setScissors(rect_ r) { Rasterizer::Scissors(r); }
		void copyColor(int dst, int src) { Textures::CopyColor(dst, src); }
		void copyDepth(int dst, int src) { Textures::CopyDepth(dst, src); }
	} gapi;
	
	#undef CreateTexture
	#define CreateTexture(name,type,format,width,height,mip,depth) name,
	namespace tex {
		enum {
			#include "..\projectFiles\texList.h"	
		};
	};

}
