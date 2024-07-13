struct {

	COMMAND(SetInputAsm, topology t) {
#include REFLECT(SetInputAsm)
		InputAssembler::IA(t);
		REFLECT_CLOSE;
	}

	COMMAND(SetRT, texture i, int level) {
#include REFLECT(SetRT)
		Textures::RenderTarget(i, level);
		REFLECT_CLOSE;
	}

	COMMAND(CreateMips) {
#include REFLECT(CreateMips)
		Textures::CreateMipMap();
		REFLECT_CLOSE;
	}


	COMMAND(SetDepthMode, depthmode i) {
#include REFLECT(SetDepthMode)
		Depth::Depth(i);
		REFLECT_CLOSE;
	}
	COMMAND(Draw, int quadcount, int instances = 1) {
#include REFLECT(Draw)
		Draw::NullDrawer(quadcount, instances);
		REFLECT_CLOSE;
	}
/*	COMMAND(SetCamera, position eye, position at, position up, unsigned int angle) {
#include REFLINK(SetCamera)
		Camera::Camera(eye, at, up, angle);
		refStackBack;
	}*/

	COMMAND(ClearRT, color4 c) {
#include REFLECT(ClearRT)
		Draw::Clear(c);
		REFLECT_CLOSE;
	}
	COMMAND(ClearRTDepth) {
#include REFLECT(ClearRTDepth)
		Draw::ClearDepth();
		REFLECT_CLOSE;
	}
	COMMAND(SetBlendMode, blendmode mode, blendop op = blendop::add) {
#include REFLECT(SetBlendMode)
		Blend::Blending(mode, op);
		REFLECT_CLOSE;
	}
	COMMAND(SetCull, cullmode i) {
#include REFLECT(SetCull)
		Rasterizer::Cull(i);
		REFLECT_CLOSE;
	}
	COMMAND(SetScissors, rect r) {
#include REFLECT(SetScissors)
		Rasterizer::Scissors(r);
		REFLECT_CLOSE;
	}
	COMMAND(CopyRTColor, texture dst, texture src) {
#include REFLECT(CopyRTColor)
		Textures::CopyColor(dst, src);
		REFLECT_CLOSE;
	}
	COMMAND(CopyRTDepth, texture dst, texture src) {
#include REFLECT(CopyRTDepth)
		Textures::CopyDepth(dst, src);
		REFLECT_CLOSE;
	}
} gApi;