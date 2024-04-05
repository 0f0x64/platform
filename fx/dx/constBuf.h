namespace ConstBuf
{
	ID3D11Buffer* vertex;
	ID3D11Buffer* pixel;

	struct frameConst
	{
		XMFLOAT4 time;
		XMFLOAT4 aspectRatio;

		XMMATRIX View;
		XMMATRIX iView;
		XMMATRIX Proj;
		XMMATRIX iProj;
		XMMATRIX iVP;

		XMFLOAT4 DepthViewVector;
		XMMATRIX DepthView;
		XMMATRIX DepthProj;
		XMMATRIX iDepthVP;
	};

	frameConst frame;

	int roundUp(int n, int r)
	{
		return 	n - (n % r) + r;
	}

	void Init()
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = roundUp(sizeof(ConstBuf::frameConst), 16);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.StructureByteStride = 16;

		HRESULT hr = device->CreateBuffer(&bd, NULL, &ConstBuf::vertex);
#if DebugMode
		if (FAILED(hr)) { Log("constant bufferV fail\n"); return; }
#endif	

		bd.ByteWidth = roundUp(sizeof(ConstBuf::frameConst), 16);
		hr = device->CreateBuffer(&bd, NULL, &ConstBuf::pixel);
#if DebugMode
		if (FAILED(hr)) { Log("constant bufferP fail\n"); return; }
#endif		
	}

	void Update()
	{
		frame.time = XMFLOAT4((float)(timer::frameBeginTime * .01), 0, 0, 0);
		context->UpdateSubresource(vertex, 0, NULL, &frame, 0, 0);
		context->UpdateSubresource(pixel, 0, NULL, &frame, 0, 0);
	}

	void Set()
	{
		context->VSSetConstantBuffers(0, 1, &vertex);
		context->PSSetConstantBuffers(0, 1, &pixel);
	}
}