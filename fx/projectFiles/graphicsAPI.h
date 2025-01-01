namespace gfx {

	API(SetInputAsm, topology topo) {
		InputAssembler::IA(topo);
	}

	API(SetRT, texture targetRT, int level) {
		Textures::RenderTarget(targetRT, level);
	}

	API(CreateMips) {
		Textures::CreateMipMap();
	}

	API(SetDepthMode, depthmode mode) {
		Depth::Depth(mode);
	}

	API(Draw, int quadcount, int instances) {
		Draw::NullDrawer(quadcount, instances);
	}
	API(SetCamera, position eye, position at, position up, unsigned int angle) {
	//		Camera::Camera(eye, at, up, angle);
	}

//	void eld(int i, float& x, float& y, float w, float lead, float sel)
	//{

	//}

	API(ClearRT, color4 color) {
		Draw::Clear(color);
	}

	API(ClearRTDepth) {
		Draw::ClearDepth();
	}
	API(SetBlendMode, blendmode mode, blendop op) {
		//regDrawer(eld);
		Blend::Blending(mode, op);
	}

	API(SetCull, cullmode mode) {
		Rasterizer::Cull(mode);
	}
	API(SetScissors, rect bbox) {
		Rasterizer::Scissors(bbox);
	}
	API(CopyRTColor, texture dst, texture src) {
		Textures::CopyColor(dst, src);
	}
	API(CopyRTDepth, texture dst, texture src) {
		Textures::CopyDepth(dst, src);
	}
}

