namespace Rasterizer 
{

	ID3D11RasterizerState* rasterState[4];

	API Cull(cullmode_ i)
	{
		context->RSSetState(rasterState[i]);
	}

	API Scissors(rect_ r)
	{
		D3D11_RECT rect = { (int)(r.x * width), (int)(r.y * height), (int)(r.x1 * width), (int)(r.y1 * height) };
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
		rasterizerState.AntialiasedLineEnable = false;
		device->CreateRasterizerState(&rasterizerState, &rasterState[0]);

		rasterizerState.CullMode = D3D11_CULL_FRONT;
		device->CreateRasterizerState(&rasterizerState, &rasterState[1]);

		rasterizerState.CullMode = D3D11_CULL_BACK;
		device->CreateRasterizerState(&rasterizerState, &rasterState[2]);

		rasterizerState.CullMode = D3D11_CULL_NONE;
		rasterizerState.FillMode = D3D11_FILL_WIREFRAME;
		device->CreateRasterizerState(&rasterizerState, &rasterState[3]);

		context->RSSetState(rasterState[0]);
		Scissors(rect_{ 0, 0, 1, 1 });
	}

}