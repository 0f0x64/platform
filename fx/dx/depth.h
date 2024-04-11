namespace Depth
{

	ID3D11DepthStencilState* pDSState[4];

	void Init()
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		// Depth test parameters
		dsDesc.DepthEnable = false;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

		// Stencil test parameters
		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing 
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create depth stencil state
		dsDesc.StencilEnable = false;

		dsDesc.DepthEnable = false;
		device->CreateDepthStencilState(&dsDesc, &pDSState[0]);//off

		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		device->CreateDepthStencilState(&dsDesc, &pDSState[1]);//read & write

		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		device->CreateDepthStencilState(&dsDesc, &pDSState[2]);//read

		dsDesc.DepthEnable = false;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		device->CreateDepthStencilState(&dsDesc, &pDSState[3]);//write
	}

	enum mode {
		off, on, readonly, writeonly
	};
	

	void Set(mode m)
	{
		context->OMSetDepthStencilState(pDSState[m], 1);
	}


	void Off()
	{
		context->OMSetDepthStencilState(pDSState[mode::off], 1);
	}

	void On()
	{
		context->OMSetDepthStencilState(pDSState[mode::on], 1);
	}

	void ReadOnly()
	{
		context->OMSetDepthStencilState(pDSState[mode::readonly], 1);
	}

	void WriteOnly()
	{
		context->OMSetDepthStencilState(pDSState[mode::writeonly], 1);
	}


}
