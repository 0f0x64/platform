namespace Draw
{ 

	void Clear(float r, float g, float b, float a)
	{
		context->ClearRenderTargetView(Textures::texture[Textures::currentRT].RenderTargetView[0][0], XMVECTORF32{ r,g,b,a });
	}

	void ClearDepth()
	{
		context->ClearDepthStencilView(Textures::texture[Textures::currentRT].DepthStencilView[0], D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void NullDrawer(int quadCount, int instances)
	{
		ConstBuf::Update(0, ConstBuf::drawerV);
		ConstBuf::SetToVertex(0);
		ConstBuf::Update(1, ConstBuf::drawerP);
		ConstBuf::SetToPixel(1);

		context->DrawInstanced(quadCount * 6, instances, 0, 0);
	}

	void LineNullDrawer(int lineCount, int instances)
	{
		ConstBuf::Update(0, ConstBuf::drawerV);
		ConstBuf::SetToVertex(0);
		ConstBuf::Update(1, ConstBuf::drawerP);
		ConstBuf::SetToPixel(1);

		ConstBuf::Update(6, ConstBuf::float4array);
		ConstBuf::SetToVertex(6);

		context->DrawInstanced(lineCount*2 , instances, 0, 0);
	}

	void Present()
	{
		Textures::UnbindAll();
		swapChain->Present(1, 0);
	}

}

