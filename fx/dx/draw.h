	struct{ 

		void Clear(float r, float g, float b, float a)
		{
			context->ClearRenderTargetView(Textures::texture[Textures::currentRT].RenderTargetView[0][0], XMVECTORF32{ r,g,b,a });
		}

		void ClearDepth()
		{
			context->ClearDepthStencilView(Textures::texture[Textures::currentRT].DepthStencilView[0], D3D11_CLEAR_DEPTH, 1.0f, 0);
		}

		void NullDrawer(int instances, int quadCount)
		{
			ConstBuf::Api.Update(3,ConstBuf::drawer);
			ConstBuf::Api.Set(3);

			context->DrawInstanced(quadCount * 6, instances, 0, 0);
		}

		void Present()
		{
			swapChain->Present(1, 0);
		}

	} Draw;

