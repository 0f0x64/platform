void paramConstBufInit();

namespace ConstBuf
{
	ID3D11Buffer* buffer[4];
	
	enum class cBuffer { reserved, camera , frame, global };

	#define constCount 32

	//b1 
	struct {
		XMMATRIX world[2];
		XMMATRIX view[2];
		XMMATRIX proj[2];
	} camera;//update per camera set

	//b2
	struct {
		XMFLOAT4 time;
		XMFLOAT4 aspect;
	} frame;//update per frame

	//b3
	XMFLOAT4 global[constCount];//update once on start

	char* cBufPtr[] = { NULL, (char*)&camera ,(char*)&frame,(char*)&global};

	int roundUp(int n, int r)
	{
		return 	n - (n % r) + r;
	}

	void Create(ID3D11Buffer* &buf, int size)
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = roundUp(size, 16);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.StructureByteStride = 16;

		HRESULT hr = device->CreateBuffer(&bd, NULL, &buf);
		LogIfError("constant buffer fail\n");
	}

	void Init()
	{
		paramConstBufInit();

		Create(buffer[(int)cBuffer::camera], sizeof(camera));
		Create(buffer[(int)cBuffer::frame], sizeof(frame));
		Create(buffer[(int)cBuffer::global], sizeof(global));
	}

	void Update(cBuffer i)
	{
		context->UpdateSubresource(buffer[(int)i], 0, NULL, cBufPtr[(int)i], 0, 0);
	}

	void Update(ID3D11Buffer* buf, auto& data)
	{
		context->UpdateSubresource(buf, 0, NULL, &data, 0, 0);
	}

	enum class target {vertex,pixel,both};

	void Set(cBuffer i, target shader)
	{
		if (shader == target::vertex || shader == target::both)
		{
			context->VSSetConstantBuffers((int)i, 1, &buffer[(int)i]);
		}

		if (shader == target::pixel || shader == target::both)
		{
			context->PSSetConstantBuffers((int)i, 1, &buffer[(int)i]);
		}

	}


	
	
} 

