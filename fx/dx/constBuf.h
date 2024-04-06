namespace ConstBuf
{

	ID3D11Buffer* vertex[5];
	ID3D11Buffer* pixel[5];

	#define constCount 32

	XMFLOAT4 frame[constCount];
	XMMATRIX object[constCount];
	XMMATRIX camera[2][constCount];
	XMFLOAT4 params[constCount];
	XMFLOAT4 external[64][constCount];

	int roundUp(int n, int r)
	{
		return 	n - (n % r) + r;
	}

	void CreateCB(int i, int size)
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = roundUp(size, 16);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.StructureByteStride = 16;

		HRESULT hr = device->CreateBuffer(&bd, NULL, &vertex[i]);
		#if DebugMode
			if (FAILED(hr)) { Log("constant bufferV fail\n"); return; }
		#endif	

		hr = device->CreateBuffer(&bd, NULL, &pixel[i]);
		#if DebugMode
			if (FAILED(hr)) { Log("constant bufferP fail\n"); return; }
		#endif		
	}

	void Init()
	{
		CreateCB(0, sizeof(frame));
		CreateCB(1, sizeof(object));
		CreateCB(2, sizeof(camera));
		CreateCB(3, sizeof(params));
		CreateCB(4, sizeof(external));
	}

	struct
	{
		void UpdateFrame()
		{
			context->UpdateSubresource(ConstBuf::vertex[0], 0, NULL, &frame, 0, 0);
			context->UpdateSubresource(ConstBuf::pixel[0], 0, NULL, &frame, 0, 0);
		}

		void UpdateObject()
		{
			context->UpdateSubresource(ConstBuf::vertex[1], 0, NULL, &object, 0, 0);
			context->UpdateSubresource(ConstBuf::pixel[1], 0, NULL, &object, 0, 0);
		}

		void UpdateCamera()
		{
			context->UpdateSubresource(ConstBuf::vertex[2], 0, NULL, &camera, 0, 0);
			context->UpdateSubresource(ConstBuf::pixel[2], 0, NULL, &camera, 0, 0);
		}

		void UpdateParams()
		{
			context->UpdateSubresource(ConstBuf::vertex[3], 0, NULL, &params, 0, 0);
			context->UpdateSubresource(ConstBuf::pixel[3], 0, NULL, &params, 0, 0);
		}

		void UpdateExternal()
		{
			context->UpdateSubresource(ConstBuf::vertex[4], 0, NULL, &external, 0, 0);
			context->UpdateSubresource(ConstBuf::pixel[4], 0, NULL, &external, 0, 0);
		}


		void Set(int i)
		{
			context->VSSetConstantBuffers(i, 1, &ConstBuf::vertex[i]);
			context->PSSetConstantBuffers(i, 1, &ConstBuf::pixel[i]);
		}
	} Api;
} 

