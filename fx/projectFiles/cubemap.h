namespace Cubemap {

	COMMAND (CalcCubemap, texture target)
	{
		#include REFLINK(CalcCubemap)

		gApi.SetBlendMode(blendmode::off, blendop::add);
		gApi.SetRT(target,0);
		gApi.SetCull(cullmode::off);
		gApi.SetDepthMode(depthmode::off);
		gApi.ClearRT( 0, 0, 0, 1 );

		vs::quad.set();
		ps::cubemapCreator.set();
		gApi.Draw(1,1);
		gApi.CreateMips();

		refStackBack;
	}

	COMMAND(ShowCubemap,texture envTexture)
	{
		#include REFLINK(ShowCubemap)

		gApi.SetBlendMode(blendmode::off, blendop::add);
		gApi.SetCull(cullmode::back);

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
		gApi.Draw(gX * gY,1);

		refStackBack;
	}

}