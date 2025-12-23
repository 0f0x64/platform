namespace Object {

	cmd(Show, 
		texture geometry;
		texture normals;
		int8u quality; 
		int pos_x;
		int pos_y;
		int pos_z;
		int glow;
		)
	{
		reflect;

		#if EditMode //dynamic limits
			auto r = max(Textures::Texture[(int)in.geometry].size.x, Textures::Texture[(int)in.geometry].size.y);
			auto mipMaps = Textures::Texture[(int)in.geometry].mipMaps;
			cmdParamDesc[cmdCounter - 1].param[2]._min = 0;
			cmdParamDesc[cmdCounter - 1].param[2]._max = max((mipMaps ? (UINT)(_log2(r)) : 0)-2,0);
		#endif

		int denom = (int)pow(2, (float)in.quality);
		float q = intToFloatDenom;
		
		int gX = Textures::Texture[(int)in.geometry].size.x / denom;
		int gY = Textures::Texture[(int)in.geometry].size.y / denom;

		vs::objViewer = {

			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(in.pos_x / q, in.pos_y / q, in.pos_z / q)),
				.gX = gX,
				.gY = gY,
				.glow_p = (float)in.glow,
			},

			.textures = {
				.positions = in.geometry,
				.normals = in.normals
				},

			.samplers = {
				.sam1Filter = filter::linear,
				.sam1AddressU = addr::wrap,
				.sam1AddressV = addr::clamp
			}
		};

		vs::objViewer.set();

		ps::basic = 
		{
			.params = {
				#if EditMode
					.hilight = cmdCounter - 1 == hilightedCmd ? 1.f : 0.f
				#else 
					.hilight = 0.f
				#endif
				}

		};

		ps::basicLow =
		{
			.params = {
				#if EditMode
					.hilight = cmdCounter - 1 == hilightedCmd ? 1.f : 0.f
				#else 
					.hilight = 0.f
				#endif
				}

		};

		if (in.glow == 1)
		{
			ps::basic.set();
		}
		else
		{
			ps::basicLow.set();
		}
		

		//Drawer::NullDrawer({(int)gX*(int)gY,1});
		if (in.glow == 0)
		{
			Drawer::NullDrawer({ 1, (int)gX * (int)gY/ 10394 });
		}
		else
		{
			Drawer::NullDrawer({ 1, (int)gX * (int)gY });
		}

		reflect_close;
	}

	unsigned int quality = 1;

	void PillarsH(int gX,int gY)
	{
		ps::basic = { .params = {.hilight = 0.f } };
		ps::basic.set();

		vs::pillars = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.glow_p = 1,
				.skipper = 1,
			},
		};

		vs::pillars.set();

		Drawer::NullDrawer({ 1, (int)gX * (int)gY });
	}

	void OuterSpaceH(int gX, int gY)
	{
		ps::basic = { .params = {.hilight = 0.f } };
		ps::basic.set();

		vs::space = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.glow_p = 1,
				.skipper = 1,
			},
		};

		vs::space.set();

		Drawer::NullDrawer({ 1, (int)gX * (int)gY });
	}

	cmd(ShowParticles)
	{
		reflect;

		int glow = 1;

		int gX = 1024 / quality;
		int gY = 1024 / quality;

		//pillars
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		PillarsH(gX,gY);
		OuterSpaceH(gX, gY);

		//pillars low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		ps::basicLow = { .params = {.hilight = 0.f } };
		ps::basicLow.set();

		vs::pillars = {
			.params = {
			.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
			.gX = gX,
			.gY = gY,
			.glow_p = 0,
			.skipper = 8000,
			},
		};

		vs::pillars.set();

		Drawer::NullDrawer({ 1, (int)gX * (int)gY / vs::pillars.params.skipper });

		reflect_close;
	}

	cmd(CalcNormals, texture srcGeomerty; texture targetNrml;)
	{
		reflect;

		RenderTarget::Set({ in.targetNrml, 0 });

		vs::quad.set();

		ps::genNormals = {

			.textures = {
				.geo = in.srcGeomerty
			},

			.samplers = {
				.sam1Filter = filter::linear,
				.sam1AddressU = addr::wrap,
				.sam1AddressV = addr::wrap
			}
		};

		ps::genNormals.set();

		Drawer::NullDrawer({ 1, 1 });
		RenderTarget::GenerateMips({});

		reflect_close;

	}

	cmd(Calc, texture targetGeo; texture targetNrml;)
	{
		reflect;

		BlendMode::Set({ blendmode::off, blendop::add });
		Culling::Set({cullmode::off});
		RenderTarget::Set({ in.targetGeo,0 });
		DepthBuf::Mode({ depthmode::off });

		//pos
		vs::quad.set();
		ps::cat.set();
		Drawer::NullDrawer({ 1, 1 });
		RenderTarget::GenerateMips({});

		//normals
		CalcNormals({ in.targetGeo, in.targetNrml });

		reflect_close;
	}

}
