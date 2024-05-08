namespace Cubemap {

	API CalcCubemap(texture_ target)
	{
		gapi.blend(blendmode::off, blendop::add);
		gapi.rt(target);
		gapi.cull(cullmode::off);
		gapi.depth(depthmode::off);
		gapi.clear(color4_{ 0, 0, 0, 1 });

		vs::quad.set();
		ps::cubemapCreator.set();
		gapi.draw(1, 1);
		gapi.mips();
	}

	API ShowCubemap(texture_ envTex)
	{
		gapi.blend(blendmode::off, blendop::add);
		gapi.cull(cullmode::back);

		ps::cubeMapViewer.textures.env = tex::env;
		ps::cubeMapViewer.samplers.sam1Filter = filter::linear;
		ps::cubeMapViewer.samplers.sam1AddressU = addr::wrap;
		ps::cubeMapViewer.samplers.sam1AddressV = addr::wrap;
		ps::cubeMapViewer.set();
		int gX = 4;
		int gY = 3;
		vs::simpleCube.params.gX = (float)gX;
		vs::simpleCube.params.gY = (float)gY;
		vs::simpleCube.set();
		gapi.draw(gX * gY);
	}

}