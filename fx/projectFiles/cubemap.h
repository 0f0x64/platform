namespace Cubemap {

	cmd(Calc, texture target;)
	{
		reflect;
		BlendMode::Set({ blendmode::off, blendop::add });
		RenderTarget::Set({in.target, 0});
		Culling::Set({cullmode::off});
		DepthBuf::Mode({depthmode::off});
		RenderTarget::Clear({0, 0, 0, 1});

		vs::quad.set();
		ps::cubemapCreator.set();
		Drawer::NullDrawer({ 1, 1 });
		RenderTarget::GenerateMips({});
		reflect_close;
	}

	cmd(Show, texture envTexture;)
	{
		reflect;

		DepthBuf::Mode({ depthmode::off });
		BlendMode::Set({ blendmode::off, blendop::add });
		Culling::Set({ cullmode::back });

		ps::cubeMapViewer = {

			.textures = {
				.env = in.envTexture
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
			.gX = gX,
			.gY = gY
		};

		vs::simpleCube.set();

		Drawer::NullDrawer({ gX * gY,1 });

		reflect_close;
	}

}
