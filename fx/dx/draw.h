namespace Draw
{
	void Clear(float r, float g, float b, float a)
	{
		context->ClearRenderTargetView(Textures::texture[Textures::currentRT].RenderTargetView[0][0], XMVECTORF32{ r,g,b,a });
	}

	void NullDrawer(int instances, int quadCount)
	{
		context->DrawInstanced(quadCount * 6, instances, 0, 0);
	}

	void Present()
	{
		swapChain->Present(1, 0);
	}

}

