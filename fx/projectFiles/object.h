namespace Object {

	cmd(Show, 
		texture geometry;
		texture normals;
		int quality; 
		int pos_x;
		int pos_y;
		int pos_z;
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
		
		ConstBuf::drawerMat = {

			.model = XMMatrixTranspose(XMMatrixTranslation(in.pos_x / q, in.pos_y / q, in.pos_z / q)),

			#if EditMode
				.hilight = cmdCounter - 1 == hilightedCmd ? 1.f : 0.f
			#else 
				.hilight = 0.f
			#endif
		};

		ConstBuf::Update(ConstBuf::cBuffer::drawerMat);

		float gX = Textures::Texture[(int)in.geometry].size.x / denom;
		float gY = Textures::Texture[(int)in.geometry].size.y / denom;

		vs::objViewer = {

			.params = {
				.gX = gX,
				.gY = gY
			},

			.textures = {
				.positions = in.geometry,
				.normals = in.normals
				},

			.samplers = {
				.sam1Filter = filter::linear,
				.sam1AddressU = addr::wrap,
				.sam1AddressV = addr::wrap
			}
		};

		vs::objViewer.set();

		ps::basic = 
		{
			.params = {
				},

			.textures = {
				.env = texture::env,
				.normals = in.normals
			},

			.samplers = {
				.sam1Filter = filter::linear,
				.sam1AddressU = addr::wrap,
				.sam1AddressV = addr::wrap
				}
		};

		ps::basic.set();

		
		/*if (dx11::Shaders::PS[(int)dx11::Shaders::pixel::basic].params)
		{
			dx11::ConstBuf::Create(dx11::Shaders::PS[(int)dx11::Shaders::pixel::basic].params, sizeof(ps::basic.params));
		}
		context->UpdateSubresource(dx11::Shaders::PS[(int)dx11::Shaders::pixel::basic].params, 0, NULL, &ps::basic.params, 0, 0);
		context->PSSetConstantBuffers(6, 1, &dx11::Shaders::PS[(int)dx11::Shaders::pixel::basic].params);
		*/


		Drawer::NullDrawer({(int)gX*(int)gY,2});

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
		DepthBuf::SetMode({ depthmode::off });

		//pos
		vs::quad.set();
		ps::obj1.set();
		Drawer::NullDrawer({ 1, 1 });
		RenderTarget::GenerateMips({});

		//normals
		CalcNormals({ in.targetGeo, in.targetNrml });

		reflect_close;
	}

}
