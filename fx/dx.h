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

	//camwera
	XMMATRIX View;
	XMMATRIX Projection;
	XMMATRIX DepthView;
	XMMATRIX DepthProjection;

	ID3D11SamplerState* pSampler[13];


	int width;
	int height;
	float aspect;
	float iaspect;

	namespace init
	{
		void Viewport()
		{
			D3D11_VIEWPORT vp;
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


		HRESULT Device()
		{
			HRESULT hr = S_OK;

			width = GetSystemMetrics(SM_CXSCREEN);
			height = GetSystemMetrics(SM_CYSCREEN);

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
				if (FAILED(hr)) {
					MessageBox(hWnd, "device not created", "dx11error", MB_OK); return S_FALSE;
				}
			#endif	

			ID3D11Texture2D* pBackBuffer = NULL;
			hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			#if DebugMode
				if (FAILED(hr)) {
					MessageBox(hWnd, "swapchain error", "dx11error", MB_OK); return S_FALSE;
				}
			#endif		

			hr = device->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
			#if DebugMode
				if (FAILED(hr)) {
					MessageBox(hWnd, "rt not created", "dx11error", MB_OK); return S_FALSE;
				}
			#endif	

			pBackBuffer->Release();

			SetTextureOptions();

			Rasterizer();

			Depth();

			BlendState();

			Viewport();

			Camera();

			return S_OK;
		}

	}

	void Clear(XMVECTORF32 col)
	{
		dx::context->ClearRenderTargetView(dx::renderTargetView, col);

	}

	void Present()
	{
		dx::swapChain->Present(1, 0);
	}
}
