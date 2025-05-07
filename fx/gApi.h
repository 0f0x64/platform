namespace InputAsm {

	cmd(set, topology topo;) {

		reflect;

		InputAssembler::IA(in.topo);

		reflect_close;
	}
}

namespace RenderTarget {

	cmd(Set, texture targetRT; int level;) {
		reflect;
		Textures::RenderTarget(in.targetRT, in.level);
		reflect_close;
	}
}

namespace Texture {

	cmd(CreateMips) {
		reflect;
		Textures::CreateMipMap();
		reflect_close;
	}
}

namespace DepthBuf {
	
	cmd(SetMode, depthmode mode;) {
		reflect;
		Depth::Depth(in.mode);
		reflect_close;
	}

	cmd(Clear) {
		reflect;
		Draw::ClearDepth();
		reflect_close;
	}
}

namespace Drawer {

	cmd(NullDrawer, int quadcount; int instances;) {
		reflect;
		Draw::NullDrawer(in.quadcount, in.instances);
		reflect_close;
	}

	cmd(Clear, int r;int g; int b;int a;)
	{
		reflect;
		Draw::Clear({in.r,in.g, in.b, in.a });
		reflect_close;
	}
}

namespace Camera
{
	cmd(Set, int eye_x; int eye_y; int eye_z; int at_x; int at_y; int at_z; int up_x; int up_y; int up_z; int angle;)
	{
		reflect;
		reflect_close;
		//		Camera::Camera(eye, at, up, angle);
	}
}

namespace BlendMode {

	cmd(Set, blendmode mode; blendop op;) {
		reflect;
		Blend::Set(in.mode, in.op);
		reflect_close;
	}
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