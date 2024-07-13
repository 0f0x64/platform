namespace Rasterizer 
{

	ID3D11RasterizerState* rasterState[4];

	void Cull(cullmode mode)
	{
		context->RSSetState(rasterState[(int)mode]);
	}

	void Scissors(rect r)
	{
		D3D11_RECT rect = { r.x,r.y,r.z,r.w };
		context->RSSetScissorRects(1, &rect);
	}

	void Init()
	{
		D3D11_RASTERIZER_DESC rasterizerState;
		rasterizerState.FillMode = D3D11_FILL_SOLID;
		rasterizerState.CullMode = D3D11_CULL_NONE;
		rasterizerState.FrontCounterClockwise = true;
		rasterizerState.DepthBias = false;
		rasterizerState.DepthBiasClamp = 0;
		rasterizerState.SlopeScaledDepthBias = 0;
		rasterizerState.DepthClipEnable = false;
		rasterizerState.ScissorEnable = true;
		rasterizerState.MultisampleEnable = false;
		rasterizerState.AntialiasedLineEnable = true;
		device->CreateRasterizerState(&rasterizerState, &rasterState[0]);

		rasterizerState.CullMode = D3D11_CULL_FRONT;
		device->CreateRasterizerState(&rasterizerState, &rasterState[1]);

		rasterizerState.CullMode = D3D11_CULL_BACK;
		device->CreateRasterizerState(&rasterizerState, &rasterState[2]);

		rasterizerState.CullMode = D3D11_CULL_NONE;
		rasterizerState.FillMode = D3D11_FILL_WIREFRAME;
		device->CreateRasterizerState(&rasterizerState, &rasterState[3]);

		context->RSSetState(rasterState[0]);
		Scissors(rect{ 0, 0, width, height });
	}

}