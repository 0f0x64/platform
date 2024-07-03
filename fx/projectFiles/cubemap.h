namespace Cubemap {

	COMMAND (CalcCubemap, texture target)
	{
		#include REFLINK(CalcCubemap)

		gapi.blend(blendmode::off, blendop::add);
		gapi.rt(target,0);
		gapi.cull(cullmode::off);
		gapi.depth(depthmode::off);
		gapi.clear(color4{ 0, 0, 0, 1 });

		vs::quad.set();
		ps::cubemapCreator.set();
		gapi.draw(1);
		gapi.mips();
	}

	COMMAND(ShowCubemap,texture envTexture)
	{
		#include REFLINK(ShowCubemap)

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