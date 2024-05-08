namespace Draw
{ 

	API Clear(color4_ c)
	{
		context->ClearRenderTargetView(Textures::texture[Textures::currentRT].RenderTargetView[0][0], XMVECTORF32{ c.r,c.g,c.b,c.a });
	}

	API ClearDepth()
	{
		context->ClearDepthStencilView(Textures::texture[Textures::currentRT].DepthStencilView[0], D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	API NullDrawer(int quadCount, int instances = 1)
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

