struct {

	COMMAND(SetInputAsm, topology t) {
#include REFLINK(SetInputAsm)
		InputAssembler::IA(t);
	}

	COMMAND(SetRT, texture i, int level) {
#include REFLINK(SetRT)
		Textures::RenderTarget(i, level);
	}

	COMMAND(CreateMips) {
#include REFLINK(CreateMips)
		Textures::CreateMipMap();
	}


	COMMAND(SetDepthMode, depthmode i) {
#include REFLINK(SetDepthMode)
		Depth::Depth(i);
	}
	COMMAND(Draw, int quadcount, int instances = 1) {
#include REFLINK(Draw)
		Draw::NullDrawer(quadcount, instances);
	}
	COMMAND(SetCamera, position eye, position at, position up, unsigned int angle) {
#include REFLINK(SetCamera)
		Camera::Camera(eye, at, up, angle);
	}
	COMMAND(ClearRT, color4 c) {
#include REFLINK(ClearRT)
		Draw::Clear(c);
	}
	COMMAND(ClearRTDepth) {
#include REFLINK(ClearRTDepth)
		Draw::ClearDepth();
	}
	COMMAND(SetBlendMode, blendmode mode, blendop op = blendop::add) {
#include REFLINK(SetBlendMode)
		Blend::Blending(mode, op);
	}
	COMMAND(SetCull, cullmode i) {
#include REFLINK(SetCull)
		Rasterizer::Cull(i);
	}
	COMMAND(SetScissors, rect r) {
#include REFLINK(SetScissors)
		Rasterizer::Scissors(r);
	}
	COMMAND(CopyRTColor, texture dst, texture src) {
#include REFLINK(CopyRTColor)
		Textures::CopyColor(dst, src);
	}
	COMMAND(CopyRTDepth, texture dst, texture src) {
#include REFLINK(CopyRTDepth)
		Textures::CopyDepth(dst, src);
	}
} gApi;