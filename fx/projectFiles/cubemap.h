namespace Cubemap {

	API(CalcCubemap, texture target)
	{
		gfx::SetBlendMode(blendmode::off, blendop::add);
		gfx::SetRT(target, 0);
		gfx::SetCull(cullmode::off);
		gfx::SetDepthMode(depthmode::off);
		gfx::ClearRT(0, 0, 0, 1);

		vs::quad.set();
		ps::cubemapCreator.set();
		gfx::Draw(1, 1);
		gfx::CreateMips();
	}

	API(ShowCubemap,texture envTexture)
	{
		gfx::SetDepthMode(depthmode::off);
		gfx::SetBlendMode(blendmode::off, blendop::add);
		gfx::SetCull(cullmode::back);

		ps::cubeMapViewer = {

			.textures = {
				.env = envTexture
				},

			.samplers {
				.sam1Filter = filter::linear,
				.sam1AddressU = addr::wrap,
				.sam1AddressV = addr::wrap
			}
		};

		ps::cubeMapViewer.set();

		int gX = 4;
		int gY = 3;

		vs::simpleCube.params = {
			.gX = (float)gX,
			.gY = (float)gY
		};

		vs::simpleCube.set();

		gfx::Draw(gX * gY,1);
	}

}
