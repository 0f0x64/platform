using namespace DirectX;

namespace dx
{
	D3D_DRIVER_TYPE				driverType = D3D_DRIVER_TYPE_NULL;
	ID3D11Device*				device = NULL;
	ID3D11DeviceContext*		context = NULL;
	IDXGISwapChain*				swapChain = NULL;
	
	ID3D11RenderTargetView*		renderTargetView = NULL;
	ID3D11RasterizerState*		rasterState[3];
	ID3D11BlendState*			blendState [4][5];

	ID3D11Texture2D*			depthStencil = NULL;
	ID3D11DepthStencilView*		depthStencilView = NULL;
	ID3D11ShaderResourceView*	depthStencilSRView = NULL;
	ID3D11DepthStencilState*	pDSState[4];

	//viewport
	D3D11_VIEWPORT vp;

	//camera
	XMMATRIX View;
	XMMATRIX Projection;
	XMMATRIX DepthView;
	XMMATRIX DepthProjection;

	ID3D11SamplerState* pSampler[13];

	//constBuf

	ID3D11Buffer* pConstantBufferV;
	ID3D11Buffer* pConstantBufferP;

	struct cbFrame
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

	//shaders

	typedef struct {
		ID3D11VertexShader* pShader;
		ID3DBlob* pBlob;
	} VertexShader;

	typedef struct {
		ID3D11PixelShader* pShader;
		ID3DBlob* pBlob;
	} PixelShader;

	VertexShader VS[255];
	PixelShader PS[255];

	ID3DBlob* pErrorBlob;

	//---------------------------

	void Log(const char* message)
	{
		OutputDebugString(message);
	}

	int width;
	int height;
	float aspect;
	float iaspect;

	namespace init
	{
		void Viewport()
		{
			vp.Width = (FLOAT)width;
			vp.Height = (FLOAT)height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			context->RSSetViewports(1, &vp);
		}

		void Rasterizer()
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

			context->RSSetState(rasterState[0]);
		}

		void BlendState()
		{
			D3D11_BLEND_DESC bSDesc;

			ZeroMemory(&bSDesc, sizeof(D3D11_BLEND_DESC));
			bSDesc.RenderTarget[0].BlendEnable = FALSE;
			bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			bSDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			bSDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bSDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			bSDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bSDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			//all additional rt's without alphablend, as blend=0 mode
			bSDesc.IndependentBlendEnable = true;
			for (int x = 1; x < 8; x++)
			{
				bSDesc.RenderTarget[x] = bSDesc.RenderTarget[0];
				bSDesc.RenderTarget[x].BlendEnable = false;
			}

			bSDesc.AlphaToCoverageEnable = false;
			//bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			//bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			for (int i = 0; i < 5; i++)
			{
				bSDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)(i + 1);
				device->CreateBlendState(&bSDesc, &blendState[0][i]);
			}

			bSDesc.RenderTarget[0].BlendEnable = TRUE;
			bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

			for (int i = 0; i < 5; i++)
			{
				bSDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)(i + 1);
				device->CreateBlendState(&bSDesc, &blendState[1][i]);
			}

			bSDesc.RenderTarget[0].BlendEnable = TRUE;
			bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			bSDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			bSDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bSDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bSDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bSDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			for (int i = 0; i < 5; i++)
			{
				bSDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)(i + 1);
				device->CreateBlendState(&bSDesc, &blendState[2][i]);
			}

			bSDesc.RenderTarget[0].BlendEnable = FALSE;
			bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bSDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			bSDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			bSDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bSDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bSDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			for (int i = 0; i < 5; i++)
			{
				bSDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)(i + 1);
				device->CreateBlendState(&bSDesc, &blendState[3][i]);
			}
		}

		void Depth()
		{
			D3D11_TEXTURE2D_DESC descDepth;
			ZeroMemory(&descDepth, sizeof(descDepth));
			descDepth.Width = width;
			descDepth.Height = height;
			descDepth.MipLevels = 1;
			descDepth.ArraySize = 1;
			descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
			descDepth.SampleDesc.Count = 1;
			descDepth.SampleDesc.Quality = 0;
			descDepth.Usage = D3D11_USAGE_DEFAULT;
			descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			descDepth.CPUAccessFlags = 0;
			descDepth.MiscFlags = 0;

			HRESULT hr = device->CreateTexture2D(&descDepth, NULL, &depthStencil);
			#if DebugMode
				if (FAILED(hr)) {
					MessageBox(hWnd, "depth texture error", "dx11error", MB_OK); return;
				}
			#endif		

			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			ZeroMemory(&descDSV, sizeof(descDSV));
			descDSV.Format = DXGI_FORMAT_D32_FLOAT;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0;
			descDSV.Flags = 0;

			hr = device->CreateDepthStencilView(depthStencil, &descDSV, &depthStencilView);
			#if DebugMode
				if (FAILED(hr)) {
					MessageBox(hWnd, "depthview error", "dx11error", MB_OK); return;
				}
			#endif

			D3D11_SHADER_RESOURCE_VIEW_DESC sr_desc;
			sr_desc.Format = DXGI_FORMAT_R32_FLOAT;
			sr_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			sr_desc.Texture2D.MostDetailedMip = 0;
			sr_desc.Texture2D.MipLevels = 1;

			hr = device->CreateShaderResourceView(depthStencil, &sr_desc, &depthStencilSRView);
			
			#if DebugMode
				if (FAILED(hr)) {
					MessageBox(hWnd, "depth shader resview error", "dx11error", MB_OK); return;
				}
			#endif

			context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

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

		void Camera()
		{
			View = XMMatrixTranslation(0, 0, 1);
			DepthView = View;
			Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);
			DepthProjection = Projection;
		}

		void SetTextureOptions()
		{
			D3D11_SAMPLER_DESC sampDesc;
			ZeroMemory(&sampDesc, sizeof(sampDesc));

			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			int filter[] = { D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR };
			int index = 0;

			for (int f = 0; f < 3; f++)
			{
				sampDesc.Filter = (D3D11_FILTER)filter[f];

				sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
				sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
				HRESULT h = dx::device->CreateSamplerState(&sampDesc, &pSampler[index++]);

				sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
				h = dx::device->CreateSamplerState(&sampDesc, &pSampler[index++]);

				sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
				sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				h = dx::device->CreateSamplerState(&sampDesc, &pSampler[index++]);

				sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				h = dx::device->CreateSamplerState(&sampDesc, &pSampler[index++]);
			}


			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			for (int x = 0; x < 4; x++) sampDesc.BorderColor[x] = 0;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
			sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
			dx::device->CreateSamplerState(&sampDesc, &pSampler[index]);

		}


		void Device()
		{
			HRESULT hr = S_OK;

			aspect = float(height) / float(width);
			iaspect = float(width) / float(height);

			DXGI_SWAP_CHAIN_DESC sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.BufferCount = 2;
			sd.BufferDesc.Width = width;
			sd.BufferDesc.Height = height;
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.BufferDesc.RefreshRate.Numerator = FRAMES_PER_SECOND;
			sd.BufferDesc.RefreshRate.Denominator = 1;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			sd.OutputWindow = hWnd;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = EditMode;

			hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, DirectXDebugMode ? D3D11_CREATE_DEVICE_DEBUG : 0, 0, 0, D3D11_SDK_VERSION, &sd, &swapChain, &device, NULL, &context);
			#if DebugMode
				if (FAILED(hr)) { Log ("device not created\n"); return; }
			#endif	

			ID3D11Texture2D* pBackBuffer = NULL;
			hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			#if DebugMode
				if (FAILED(hr)) { Log("swapchain error\n"); return; }
			#endif		

			hr = device->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
			#if DebugMode
				if (FAILED(hr)) {Log ("rt not created\n"); return; }
			#endif	

			pBackBuffer->Release();

		}

		int roundUp(int n, int r)
		{
			return 	n - (n % r) + r;
		}

		void ConstBuf()
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = roundUp(sizeof(cbFrame), 16);
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.StructureByteStride = 16;

			HRESULT hr = device->CreateBuffer(&bd, NULL, &pConstantBufferV);
			#if DebugMode
				if (FAILED(hr)) { Log ("constant bufferV fail\n"); return; }
			#endif	

			bd.ByteWidth = roundUp(sizeof(cbFrame), 16);
			hr = device->CreateBuffer(&bd, NULL, &pConstantBufferP);
			#if DebugMode
				if (FAILED(hr)) { Log("constant bufferP fail\n"); return; }
			#endif		
		}

		void Init()
		{
			Device();
			SetTextureOptions();
			Rasterizer();
			Depth();
			BlendState();
			Viewport();
			Camera();
			ConstBuf();
		}

	}

#if EditMode

	void CompilerLog(LPCWSTR source, HRESULT hr,const char* message)
	{
		#if DebugMode
			if (FAILED(hr)) 
			{ 
				Log((char*)pErrorBlob->GetBufferPointer());
			}
			else
			{
				char shaderName[1024];
				WideCharToMultiByte(CP_ACP, NULL, source, -1, shaderName, sizeof(shaderName), NULL, NULL);

				Log(message);
				Log((char*)shaderName);
				Log("\n");
			}
		#endif
	}

	wchar_t shaderPathW[MAX_PATH];

	LPCWSTR nameToPatchLPCWSTR(const char* name)
	{
		char path[MAX_PATH];
		strcpy(path, shadersPath);
		strcat(path, name);

		int len = MultiByteToWideChar(CP_ACP, 0, path, -1, NULL, 0);
		MultiByteToWideChar(CP_ACP, 0, path, -1, shaderPathW, len);

		return shaderPathW;
	}

	void CompileVertexShaderFromFile(VertexShader* shader, const char* name)
	{
		LPCWSTR source = nameToPatchLPCWSTR(name);

		HRESULT hr = S_OK;

		hr = D3DCompileFromFile(source, NULL, NULL, "VS", "vs_4_1", NULL, NULL, &shader->pBlob, &pErrorBlob);
		CompilerLog(source,hr,"vertex shader compiled: ");

		if (hr == S_OK)
		{
			hr = device->CreateVertexShader(shader->pBlob->GetBufferPointer(), shader->pBlob->GetBufferSize(), NULL, &shader->pShader);
			#if DebugMode
				if (FAILED(hr)) { Log("vs creation fail\n"); return; }
			#endif
		}

	}

	void CompilePixelShaderFromFile(PixelShader* shader, const char* name)
	{
		LPCWSTR source = nameToPatchLPCWSTR(name);

		HRESULT hr = S_OK;

		hr = D3DCompileFromFile(source, NULL, NULL, "PS", "ps_4_1", NULL, NULL, &shader->pBlob, &pErrorBlob);
		CompilerLog(source, hr,"pixel shader compiled: ");

		if (hr == S_OK)
		{
			hr = device->CreatePixelShader(shader->pBlob->GetBufferPointer(), shader->pBlob->GetBufferSize(), NULL, &shader->pShader);
			#if DebugMode
				if (FAILED(hr)) { Log("vs creation fail\n"); return; }
			#endif
		}

	}

#else

	void CompileVertexShader(int n, const char* shaderText)
	{
		HRESULT hr = S_OK;

		VS[n].pBlob = NULL;
		hr = D3DCompile(shaderText, strlen(shaderText), NULL, NULL, NULL, "VS", "vs_4_1", D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR, NULL, &VS[n].pBlob, &pErrorBlob);

		#if DebugMode
			if (FAILED(hr)) { Log((char*)pErrorBlob->GetBufferPointer()); }
		#endif	

		if (hr == S_OK)
		{
			if (VS[n].pShader) VS[n].pShader->Release();

			hr = device->CreateVertexShader(VS[n].pBlob->GetBufferPointer(), VS[n].pBlob->GetBufferSize(), NULL, &VS[n].pShader);

			#if DebugMode
			if (FAILED(hr))
			{
				if (VS[n].pShader) VS[n].pShader->Release();
				Log("vs fail");
			}
			#endif		
		}

	}


	void CompilePixelShader(int n, const char* shaderText)
	{
		HRESULT hr = S_OK;

		PS[n].pBlob = NULL;
		hr = D3DCompile(shaderText, strlen(shaderText), NULL, NULL, NULL, "PS", "ps_4_1", D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR, NULL, &PS[n].pBlob, &pErrorBlob);

		#if DebugMode
			if (hr != S_OK) { Log("vs fail");}
		#endif	

			if (hr == S_OK)
			{
				if (PS[n].pShader) PS[n].pShader->Release();
				hr = device->CreatePixelShader(PS[n].pBlob->GetBufferPointer(), PS[n].pBlob->GetBufferSize(), NULL, &PS[n].pShader);

				#if DebugMode
					if (FAILED(hr)) { Log("ps fail\n"); }
				#endif

			}
	}

#endif

	void Clear(XMVECTORF32 col)
	{
		context->ClearRenderTargetView(renderTargetView, col);
	}

	void Present()
	{
		swapChain->Present(1, 0);
	}

	//todo: check previously setted shader, same for IA, const, etc
	void SetVS(int n)
	{
		context->VSSetShader(VS[n].pShader, NULL, 0);
	}

	void SetPS(int n)
	{
		context->PSSetShader(PS[n].pShader, NULL, 0);
	}


	void SetRT()
	{
		context->RSSetViewports(1, &vp);
		context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	}

	void SetZBuffer()
	{
		context->OMSetDepthStencilState(dx::pDSState[0], 1);
	}

	void SetIA()
	{
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(NULL);
		context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	}

	void SetCB()
	{
		context->VSSetConstantBuffers(0, 1, &pConstantBufferV);
		context->PSSetConstantBuffers(0, 1, &pConstantBufferP);
	}

	void NullDrawer(int instances, int quadCount)
	{
		context->DrawInstanced(quadCount * 6, instances, 0, 0);
	}
}
