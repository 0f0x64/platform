struct {

	COMMAND(SetInputAsm, topology t) {
#include REFLINK(SetInputAsm)
		InputAssembler::IA(t);
		refStackBack;
	}

	COMMAND(SetRT, texture i, int level) {
#include REFLINK(SetRT)
		Textures::RenderTarget(i, level);
		refStackBack;
	}

	COMMAND(CreateMips) {
#include REFLINK(CreateMips)
		Textures::CreateMipMap();
		refStackBack;
	}


	COMMAND(SetDepthMode, depthmode i) {
#include REFLINK(SetDepthMode)
		Depth::Depth(i);
		refStackBack;
	}
	COMMAND(Draw, int quadcount, int instances = 1) {
#include REFLINK(Draw)
		Draw::NullDrawer(quadcount, instances);
		refStackBack;
	}
	COMMAND(SetCamera, position eye, position at, position up, unsigned int angle) {
#include REFLINK(SetCamera)
		Camera::Camera(eye, at, up, angle);
		refStackBack;
	}
	COMMAND(ClearRT, color4 c) {
#include REFLINK(ClearRT)
		Draw::Clear(c);
		refStackBack;
	}
	COMMAND(ClearRTDepth) {
#include REFLINK(ClearRTDepth)
		Draw::ClearDepth();
		refStackBack;
	}
	COMMAND(SetBlendMode, blendmode mode, blendop op = blendop::add) {
#include REFLINK(SetBlendMode)
		Blend::Blending(mode, op);
		refStackBack;
	}
	COMMAND(SetCull, cullmode i) {
#include REFLINK(SetCull)
		Rasterizer::Cull(i);
		refStackBack;
	}
	COMMAND(SetScissors, rect r) {
#include REFLINK(SetScissors)
		Rasterizer::Scissors(r);
		refStackBack;
	}
	COMMAND(CopyRTColor, texture dst, texture src) {
#include REFLINK(CopyRTColor)
		Textures::CopyColor(dst, src);
		refStackBack;
	}
	COMMAND(CopyRTDepth, texture dst, texture src) {
#include REFLINK(CopyRTDepth)
		Textures::CopyDepth(dst, src);
		refStackBack;
	}
} gApi;