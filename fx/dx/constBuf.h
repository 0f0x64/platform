namespace ConstBuf
{

	ID3D11Buffer* buffer[5];

	#define constCount 32

	XMFLOAT4 global[constCount];//update once per start
	XMFLOAT4 frame[constCount];//update per frame
	XMMATRIX camera[2][3];//update per camera set
	XMFLOAT4 drawer[constCount];//update per draw call
	XMMATRIX drawerMat[constCount];//update per draw call
		
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

		HRESULT hr = device->CreateBuffer(&bd, NULL, &buffer[i]);
		#if DebugMode
			if (FAILED(hr)) { Log("constant bufferV fail\n"); return; }
		#endif	
	}

	void Init()
	{
		CreateCB(0, sizeof(global));
		CreateCB(1, sizeof(frame));
		CreateCB(2, sizeof(camera));
		CreateCB(3, sizeof(drawer));
		CreateCB(4, sizeof(drawerMat));
	}

	struct
	{
		template <typename T>
		void Update(int i, T* data)
		{
			context->UpdateSubresource(ConstBuf::buffer[i], 0, NULL, data, 0, 0);
		}


		void Set(int i)
		{
			context->VSSetConstantBuffers(i, 1, &ConstBuf::buffer[i]);
			context->PSSetConstantBuffers(i, 1, &ConstBuf::buffer[i]);
		}
	} Api;
} 

