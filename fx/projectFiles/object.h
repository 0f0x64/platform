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
		
		ConstBuf::drawerMat.model = XMMatrixTranspose(XMMatrixTranslation(pos.x/q, pos.y/q, pos.z/q));

		#if EditMode
			ConstBuf::drawerMat.hilight = cmdCounter - 1 == hilightedCmd ? 1.f : 0.f;
		#else 
			ConstBuf::drawerMat.hilight = 0.f;
		#endif

		ConstBuf::UpdateDrawerMat();

		float gX = Textures::Texture[(int)geometry].size.x / denom;
		float gY = Textures::Texture[(int)geometry].size.y / denom;

		vs::objViewer.textures.positions = geometry;
		vs::objViewer.textures.normals = normals;
		vs::objViewer.samplers.sam1Filter = filter::linear;
		vs::objViewer.samplers.sam1AddressU = addr::wrap;
		vs::objViewer.samplers.sam1AddressV = addr::clamp;
		vs::objViewer.params.gX = gX;
		vs::objViewer.params.gY = gY;
		vs::objViewer.set();

		ps::basic.textures.env = texture::env;
		ps::basic.textures.normals = normals;
		ps::basic.samplers.sam1Filter = filter::linear;
		ps::basic.samplers.sam1AddressU = addr::wrap;
		ps::basic.samplers.sam1AddressV = addr::wrap;

		ps::basic.set();
		gfx::Draw((int)gX*(int)gY, 1);
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
		gfx::SetRT(targetNrml,0);
		vs::quad.set();

		ps::genNormals.samplers.sam1Filter = filter::linear;
		ps::genNormals.samplers.sam1AddressU = addr::wrap;
		ps::genNormals.samplers.sam1AddressV = addr::wrap;

		ps::genNormals.textures.geo = targetGeo;
		ps::genNormals.set();

		gfx::Draw(1,1);
		gfx::CreateMips();
	}

}
