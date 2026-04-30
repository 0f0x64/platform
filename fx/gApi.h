namespace InputAsm {

	cmd(Set, topology topo) {

		reflect;

		InputAssembler::IA(in.topo);

	}
}

namespace RenderTarget {

	cmd(Set, texture targetRT, int level) {
		reflect;
		Textures::RenderTarget(in.targetRT, in.level);

	}

	cmd(GenerateMips) {
		reflect;
		Textures::CreateMipMap();

	}

	cmd(Clear, int r, int g, int b, int a)
	{
		reflect;
		Draw::Clear({ (float)in.r,(float)in.g, (float)in.b, (float)in.a });

	}
}

namespace Copy {

	cmd(Color, texture dst, texture src) 
	{
		reflect;
		Textures::CopyColor(in.dst, in.src);

	}

	cmd(Depth, texture dst, texture src) {
		reflect;
		Textures::CopyDepth(in.dst, in.src);

	}
}

namespace DepthBuf {
	
	cmd(Mode, depthmode mode) {
		reflect;
		Depth::Depth(in.mode);

	}

	cmd(Clear) {
		reflect;
		Draw::ClearDepth();

	}
}

namespace Drawer {

	cmd(NullDrawer, int quadcount, int instances) {
		reflect;

		Draw::NullDrawer(in.quadcount, in.instances);
	}

	cmd(NullDrawerTri, int tricount, int instances) {
		reflect;

		Draw::NullDrawerTri(in.tricount, in.instances);

	}

	cmd(NullDrawerTri, int tricount; int instances;) {
		reflect;

		Draw::NullDrawerTri(in.tricount, in.instances);
		reflect_close;
	}


}

namespace Cam
{
	cmd(Set, int eye_x, int eye_y, int eye_z, int at_x, int at_y, int at_z, int up_x, int up_y, int up_z, int angle)
	{
		reflect;
		//		Camera::Camera(eye, at, up, angle);

	}
}

namespace BlendMode {

	cmd(Set, blendmode mode, blendop op) {
		reflect;
		Blend::Set(in.mode, in.op);

	}
}

namespace Culling {

	cmd(Set, cullmode mode)
	{
		reflect;
		Rasterizer::Cull(in.mode);

	}
}

namespace Scissors {

	cmd(Set, int x, int y, int x1, int y1)
	{
		reflect;
		Rasterizer::Scissors({ (float)in.x ,(float)in.y,(float)in.x1,(float)in.y1});

	}
}

