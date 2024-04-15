namespace Blend
{

	ID3D11BlendState* blendState[3][5];
	D3D11_BLEND_DESC bSDesc;

	void CreateMixStates(int j)
	{
		for (int i = 0; i < 5; i++)
		{
			bSDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)(i + 1);
			HRESULT hr = device->CreateBlendState(&bSDesc, &blendState[j][i]);
			LogIfError("CreateblendState error\n");
		}
	}

	void Init()
	{
		ZeroMemory(&bSDesc, sizeof(D3D11_BLEND_DESC));

		//all additional rt's without alphablend!
		for (int x = 0; x < 8; x++)
		{
			bSDesc.RenderTarget[x].BlendEnable = false;
			bSDesc.RenderTarget[x].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}

		bSDesc.AlphaToCoverageEnable = false;
		bSDesc.IndependentBlendEnable = true;

		CreateMixStates(0);

		bSDesc.RenderTarget[0].BlendEnable = TRUE;
		bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		bSDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bSDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bSDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		//NO ALPHA
		CreateMixStates(1);

		//ALPHA 
		bSDesc.RenderTarget[0].BlendEnable = TRUE;
		bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		CreateMixStates(2);

		float blendFactor[4] = { .0f,.0f,.0f,.0f };
		context->OMSetBlendState(blendState[0][0], blendFactor, 0xffffffff);
	}

	void Set(int m, int blend)
	{
		float blendFactor[4] = { .0f,.0f,.0f,.0f };
		context->OMSetBlendState(blendState[m][blend], blendFactor, 0xffffffff);
	}
} 