namespace Object {

	API ShowObject(texture geometry, texture normals, unsigned int quality, position pos)
	{
		int denom = (int)pow(2, quality);
		float q = intToFloatDenom;
		
		ConstBuf::drawerMat.model = XMMatrixTranspose(XMMatrixTranslation(pos.x/q, pos.y/q, pos.z/q));
		ConstBuf::UpdateDrawerMat();

		float gX = Textures::Texture[(int)texture::obj1pos].size.x / denom;
		float gY = Textures::Texture[(int)texture::obj1pos].size.y / denom;

		vs::objViewer.textures.positions = geometry;
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
		gapi.draw((int)gX * (int)gY);
	}

	API CalcObject(texture targetGeo, texture targetNrml)
	{
		gapi.blend(blendmode::off, blendop::add);
		gapi.cull(cullmode::off);
		gapi.rt(targetGeo);
		gapi.depth(depthmode::off);

		//pos
		vs::quad.set();
		ps::obj1.set();
		gapi.draw(1);
		gapi.mips();

		//normals
		gapi.rt(targetNrml);
		vs::quad.set();

		ps::genNormals.samplers.sam1Filter = filter::linear;
		ps::genNormals.samplers.sam1AddressU = addr::wrap;
		ps::genNormals.samplers.sam1AddressV = addr::wrap;

		ps::genNormals.textures.geo = targetGeo;
		ps::genNormals.set();
		gapi.draw(1);
		gapi.mips();
	}

}