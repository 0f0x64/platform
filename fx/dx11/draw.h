namespace Draw
{ 

	void Clear(color4 color)
	{
		context->ClearRenderTargetView(Textures::Texture[Textures::currentRT].RenderTargetView[0][0], XMVECTORF32{ color.x/255.f,color.y / 255.f,color.z / 255.f,color.w / 255.f });
	}

	void ClearDepth()
	{
		context->ClearDepthStencilView(Textures::Texture[Textures::currentRT].DepthStencilView[0], D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void NullDrawer(int quadCount, unsigned int instances = 1)
	{
	/*	ConstBuf::Update(ConstBuf::cBuffer::drawerV);
		ConstBuf::ConstToVertex(ConstBuf::cBuffer::drawerV);
		ConstBuf::Update(ConstBuf::cBuffer::drawerP);
		ConstBuf::ConstToPixel(ConstBuf::cBuffer::drawerP);*/

		context->DrawInstanced(quadCount * 6, instances, 0, 0);
	}

	void Present()
	{
		Textures::UnbindAll();
		swapChain->Present(1, 0);
	}

}

