namespace InputAsm {

	cmd(Set, topology topo;) {

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

	cmd(GenerateMips) {
		reflect;
		Textures::CreateMipMap();
		reflect_close;
	}

	cmd(Clear, int r; int g; int b; int a;)
	{
		reflect;
		Draw::Clear({ in.r,in.g, in.b, in.a });
		reflect_close;
	}
}

namespace Copy {

	cmd(Color, texture dst; texture src;) 
	{
		reflect;
		Textures::CopyColor(in.dst, in.src);
		reflect_close;
	}

	cmd(Depth, texture dst; texture src;) {
		reflect;
		Textures::CopyDepth(in.dst, in.src);
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


}

namespace Cam
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

namespace Culling {

	cmd(Set, cullmode mode;)
	{
		reflect;
		Rasterizer::Cull(in.mode);
		reflect_close;
	}
}

namespace Scissors {

	cmd(Set, int x; int y; int x1; int y1;)
	{
		reflect;
		Rasterizer::Scissors({ in.x ,in.y,in.x1,in.y1});
		reflect_close;
	}
}

