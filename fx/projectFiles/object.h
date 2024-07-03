namespace Object {

	COMMAND(ShowObject,texture geometry, texture normals, unsigned int quality, position pos)
	{
		#include REFLINK(ShowObject)

		int denom = (int)pow(2, quality);
		float q = intToFloatDenom;
		
		ConstBuf::drawerMat.model = XMMatrixTranspose(XMMatrixTranslation(pos.x/q, pos.y/q, pos.z/q));
		ConstBuf::drawerMat.hilight = cmdCounter-1 == hilightedCmd ? 1.f : 0.f;
		ConstBuf::UpdateDrawerMat();

		float gX = Textures::Texture[(int)texture::obj1pos].size.x / denom;
		float gY = Textures::Texture[(int)texture::obj1pos].size.y / denom;

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
		gapi.draw((int)gX * (int)gY);
		
	}

	COMMAND(CalcObject,texture targetGeo, texture targetNrml)
	{
		#include REFLINK(CalcObject)

		gApi.SetBlendMode(blendmode::off, blendop::add);
		gApi.SetCull(cullmode::off);
		gApi.SetRT(targetGeo,0);
		gApi.SetDepthMode(depthmode::off);

		//pos
		vs::quad.set();
		ps::obj1.set();
		gApi.Draw(1,1);
		gApi.CreateMips();

		//normals
		gApi.SetRT(targetNrml,0);
		vs::quad.set();

		ps::genNormals.samplers.sam1Filter = filter::linear;
		ps::genNormals.samplers.sam1AddressU = addr::wrap;
		ps::genNormals.samplers.sam1AddressV = addr::wrap;

		ps::genNormals.textures.geo = targetGeo;
		ps::genNormals.set();

		gApi.Draw(1,1);
		gApi.CreateMips();
	}

}