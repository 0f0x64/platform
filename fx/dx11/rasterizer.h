namespace Rasterizer 
{

	ID3D11RasterizerState* rasterState[4];

	void Cull(cullmode mode)
	{
		context->RSSetState(rasterState[(int)mode]);
	}

	void Scissors(float4 r)
	{
		D3D11_RECT rect = { r.x,r.y,r.z,r.w };
		context->RSSetScissorRects(1, &rect);
	}

	void Init()
	{
		D3D11_RASTERIZER_DESC rasterizerState = {
			.FillMode = D3D11_FILL_SOLID,
			.CullMode = D3D11_CULL_NONE,
			.FrontCounterClockwise = true,
			.DepthBias = false,
			.DepthBiasClamp = 0,
			.SlopeScaledDepthBias = 0,
			.DepthClipEnable =  false,
			.ScissorEnable = true,
			.MultisampleEnable =  false,
			.AntialiasedLineEnable = true
		};
		
		device->CreateRasterizerState(&rasterizerState, &rasterState[0]);

		rasterizerState.CullMode = D3D11_CULL_FRONT;
		device->CreateRasterizerState(&rasterizerState, &rasterState[1]);

		rasterizerState.CullMode = D3D11_CULL_BACK;
		device->CreateRasterizerState(&rasterizerState, &rasterState[2]);

		rasterizerState.CullMode = D3D11_CULL_NONE;
		rasterizerState.FillMode = D3D11_FILL_WIREFRAME;
		device->CreateRasterizerState(&rasterizerState, &rasterState[3]);

		context->RSSetState(rasterState[0]);
		Scissors(float4{ 0, 0, (float)width, (float)height });
	}

}