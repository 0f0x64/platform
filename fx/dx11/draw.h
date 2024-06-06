namespace Draw
{ 

	API Clear(color4 color)
	{
		context->ClearRenderTargetView(Textures::Texture[Textures::currentRT].RenderTargetView[0][0], XMVECTORF32{ color.x/255.f,color.y / 255.f,color.z / 255.f,color.w / 255.f });
	}

	API ClearDepth()
	{
		context->ClearDepthStencilView(Textures::Texture[Textures::currentRT].DepthStencilView[0], D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	API NullDrawer(int quadCount, unsigned int instances = 1)
	{
		ConstBuf::Update(0, ConstBuf::drawerV);
		ConstBuf::ConstToVertex(0);
		ConstBuf::Update(1, ConstBuf::drawerP);
		ConstBuf::ConstToPixel(1);

		context->DrawInstanced(quadCount * 6, instances, 0, 0);
	}

	void Present()
	{
		Textures::UnbindAll();
		swapChain->Present(1, 0);
	}

}

