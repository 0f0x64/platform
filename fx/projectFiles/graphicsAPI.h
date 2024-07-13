struct {

	COMMAND(SetInputAsm, topology topo) {
#include REFLECT(SetInputAsm)
		InputAssembler::IA(topo);
		REFLECT_CLOSE;
	}

	COMMAND(SetRT, texture targetRT, int level) {
#include REFLECT(SetRT)
		Textures::RenderTarget(targetRT, level);
		REFLECT_CLOSE;
	}

	COMMAND(CreateMips) {
#include REFLECT(CreateMips)
		Textures::CreateMipMap();
		REFLECT_CLOSE;
	}

	COMMAND(SetDepthMode, depthmode mode) {
#include REFLECT(SetDepthMode)
		Depth::Depth(mode);
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

	COMMAND(ClearRT, color4 color) {
#include REFLECT(ClearRT)
		Draw::Clear(color);
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
	COMMAND(SetCull, cullmode mode) {
#include REFLECT(SetCull)
		Rasterizer::Cull(mode);
		REFLECT_CLOSE;
	}
	COMMAND(SetScissors, rect bbox) {
#include REFLECT(SetScissors)
		Rasterizer::Scissors(bbox);
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