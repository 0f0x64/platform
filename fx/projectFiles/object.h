namespace Object {

	API(ShowObject,texture geometry, texture normals, unsigned int quality, position pos)
	{
		#if EditMode //dynamic limits
			auto r = max(Textures::Texture[(int)geometry].size.x, Textures::Texture[(int)geometry].size.y);
			auto mipMaps = Textures::Texture[(int)geometry].mipMaps;
			cmdParamDesc[cmdCounter - 1].param[2]._min = 0;
			cmdParamDesc[cmdCounter - 1].param[2]._max = max((mipMaps ? (UINT)(_log2(r)) : 0)-2,0);
		#endif

		int denom = (int)pow(2, (float)quality);
		float q = intToFloatDenom;
		
		ConstBuf::drawerMat = {

			.model = XMMatrixTranspose(XMMatrixTranslation(pos.x / q, pos.y / q, pos.z / q)),

			#if EditMode
				.hilight = cmdCounter - 1 == hilightedCmd ? 1.f : 0.f
			#else 
				.hilight = 0.f
			#endif
		};

		ConstBuf::Update(ConstBuf::cBuffer::drawerMat);

		float gX = Textures::Texture[(int)geometry].size.x / denom;
		float gY = Textures::Texture[(int)geometry].size.y / denom;

		vs::objViewer = {

			.params = {
				.gX = gX,
				.gY = gY
			},

			.textures = {
				.positions = geometry,
				.normals = normals
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
				.normals = normals
			},

			.samplers = {
				.sam1Filter = filter::linear,
				.sam1AddressU = addr::wrap,
				.sam1AddressV = addr::wrap
				}
		};

		ps::basic.set();

		gfx::Draw((int)gX*(int)gY, 1);
	}

	API(CalcNormals, texture srcGeomerty, texture targetNrml)
	{
		gfx::SetRT(targetNrml, 0);

		vs::quad.set();

		ps::genNormals = {

			.textures = {
				.geo = srcGeomerty
			},

			.samplers = {
				.sam1Filter = filter::linear,
				.sam1AddressU = addr::wrap,
				.sam1AddressV = addr::wrap
			}
		};

		ps::genNormals.set();

		gfx::Draw(1, 1);
		gfx::CreateMips();

	}

	API(CalcObject,texture targetGeo, texture targetNrml)
	{
		gfx::SetBlendMode(blendmode::off, blendop::add);
		gfx::SetCull(cullmode::off);
		gfx::SetRT(targetGeo,0);
		gfx::SetDepthMode(depthmode::off);

		//pos
		vs::quad.set();
		ps::obj1.set();
		gfx::Draw(1, 1);
		gfx::CreateMips();

		//normals
		CalcNormals(targetGeo, targetNrml);
	}

}
