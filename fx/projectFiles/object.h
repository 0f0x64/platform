namespace Object {

	API ShowObject(texture_ geometry, texture_ normals, int quality, position_ pos)
	{
		int denom = (int)pow(2, quality);
		
		ConstBuf::drawerMat.model = XMMatrixTranspose(XMMatrixTranslation(pos.x, pos.y, pos.z));
		ConstBuf::UpdateDrawerMat();

		float gX = Textures::texture[tex::obj1pos].size.x / denom;
		float gY = Textures::texture[tex::obj1pos].size.y / denom;

		vs::objViewer.textures.positions = geometry;
		vs::objViewer.samplers.sam1Filter = filter::linear;
		vs::objViewer.samplers.sam1AddressU = addr::wrap;
		vs::objViewer.samplers.sam1AddressV = addr::clamp;
		vs::objViewer.params.gX = gX;
		vs::objViewer.params.gY = gY;
		vs::objViewer.set();

		ps::basic.textures.env = tex::env;
		ps::basic.textures.normals = normals;
		ps::basic.samplers.sam1Filter = filter::linear;
		ps::basic.samplers.sam1AddressU = addr::wrap;
		ps::basic.samplers.sam1AddressV = addr::wrap;

		ps::basic.set();
		gapi.draw((int)gX * (int)gY);
	}

	API CalcObject(texture_ targetGeoTexture, texture_ targetNrmlTexture)
	{
		gapi.blend(blendmode::off, blendop::add);
		gapi.cull(cullmode::off);
		gapi.rt(targetGeoTexture);
		gapi.depth(depthmode::off);

		//pos
		vs::quad.set();
		ps::obj1.set();
		gapi.draw(1);
		gapi.mips();

		//normals
		gapi.rt(targetNrmlTexture);
		vs::quad.set();

		ps::genNormals.samplers.sam1Filter = filter::linear;
		ps::genNormals.samplers.sam1AddressU = addr::wrap;
		ps::genNormals.samplers.sam1AddressV = addr::wrap;

		ps::genNormals.textures.geo = targetGeoTexture;
		ps::genNormals.set();
		gapi.draw(1);
		gapi.mips();
	}

}