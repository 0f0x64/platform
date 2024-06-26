namespace Cubemap {

	API CalcCubemap(texture target)
	{
		gapi.blend(blendmode::off, blendop::add);
		gapi.rt(target);
		gapi.cull(cullmode::off);
		gapi.depth(depthmode::off);
		gapi.clear(color4{ 0, 0, 0, 1 });

		vs::quad.set();
		ps::cubemapCreator.set();
		gapi.draw(1, 1);
		gapi.mips();
	}

	API ShowCubemap(texture envTexture)
	{
		gapi.blend(blendmode::off, blendop::add);
		gapi.cull(cullmode::back);

		ps::cubeMapViewer.textures.env = envTexture;
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