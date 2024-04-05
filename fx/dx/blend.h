namespace Blend
{

	ID3D11BlendState* blendState[4][5];

	void Init()
	{
		D3D11_BLEND_DESC bSDesc;

		ZeroMemory(&bSDesc, sizeof(D3D11_BLEND_DESC));
		bSDesc.RenderTarget[0].BlendEnable = FALSE;
		bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		bSDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bSDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bSDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bSDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bSDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		//all additional rt's without alphablend, as blend=0 mode
		bSDesc.IndependentBlendEnable = true;
		for (int x = 1; x < 8; x++)
		{
			bSDesc.RenderTarget[x] = bSDesc.RenderTarget[0];
			bSDesc.RenderTarget[x].BlendEnable = false;
		}

		bSDesc.AlphaToCoverageEnable = false;
		//bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		//bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		for (int i = 0; i < 5; i++)
		{
			bSDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)(i + 1);
			device->CreateBlendState(&bSDesc, &blendState[0][i]);
		}

		bSDesc.RenderTarget[0].BlendEnable = TRUE;
		bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

		for (int i = 0; i < 5; i++)
		{
			bSDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)(i + 1);
			device->CreateBlendState(&bSDesc, &blendState[1][i]);
		}

		bSDesc.RenderTarget[0].BlendEnable = TRUE;
		bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bSDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bSDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bSDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		bSDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bSDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		for (int i = 0; i < 5; i++)
		{
			bSDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)(i + 1);
			device->CreateBlendState(&bSDesc, &blendState[2][i]);
		}

		bSDesc.RenderTarget[0].BlendEnable = FALSE;
		bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		bSDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bSDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		bSDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		bSDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bSDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		for (int i = 0; i < 5; i++)
		{
			bSDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)(i + 1);
			device->CreateBlendState(&bSDesc, &blendState[3][i]);
		}
	}
}