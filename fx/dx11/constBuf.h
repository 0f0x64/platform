namespace ConstBuf
{

	ID3D11Buffer* buffer[6];

	#define constCount 32

	//b0 - use "params" label in shader
	float drawerV[constCount];//update per draw call

	//b1 - use "params" label in shader
	float drawerP[constCount];//update per draw call

	//b2
	struct {
		XMMATRIX model;
	} drawerMat;//update per draw call

	//b3 
	struct {
		XMMATRIX world[2];
		XMMATRIX view[2];
		XMMATRIX proj[2];
	} camera;//update per camera set

	//b4
	struct {
		float time;
	} frame;//update per frame

	//b5
	XMFLOAT4 global[constCount];//update once on start

	int roundUp(int n, int r)
	{
		return 	n - (n % r) + r;
	}

	void CreateCB(ID3D11Buffer* &buf, int size)
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = roundUp(size, 16);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.StructureByteStride = 16;

		HRESULT hr = device->CreateBuffer(&bd, NULL, &buf);
		LogIfError("constant bufferV fail\n");
	}

	void Init()
	{
		CreateCB(buffer[0], sizeof(drawerV));
		CreateCB(buffer[1], sizeof(drawerP));
		CreateCB(buffer[2], sizeof(drawerMat));
		CreateCB(buffer[3], sizeof(camera));
		CreateCB(buffer[4], sizeof(frame));
		CreateCB(buffer[5], sizeof(global));
	}

	template <typename T>
	void Update(int i, T* data)
	{
		context->UpdateSubresource(buffer[i], 0, NULL, data, 0, 0);
	}

	void UpdateFrame()
	{
		context->UpdateSubresource(buffer[4], 0, NULL, &frame, 0, 0);
	}

	void UpdateCamera()
	{
		context->UpdateSubresource(ConstBuf::buffer[3], 0, NULL, &camera, 0, 0);
	}

	void SetToVertex(int i)
	{
		context->VSSetConstantBuffers(i, 1, &buffer[i]);
	}

	void SetToPixel(int i)
	{
		context->PSSetConstantBuffers(i, 1, &buffer[i]);
	}


} 

