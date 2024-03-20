using namespace DirectX;

namespace dx
{
	D3D_DRIVER_TYPE g_driverType = D3D_DRIVER_TYPE_NULL;
	ID3D11Device* g_pd3dDevice = NULL;
	ID3D11DeviceContext* g_pImmediateContext = NULL;
	IDXGISwapChain* g_pSwapChain = NULL;
	ID3D11RenderTargetView* g_pRenderTargetView = NULL;

	float width;
	float height;
	float aspect;
	float iaspect;


	HRESULT InitDevice()
	{
		HRESULT hr = S_OK;

		width = (float)GetSystemMetrics(SM_CXSCREEN);
		height = (float)GetSystemMetrics(SM_CYSCREEN);

		aspect = height/width;
		iaspect = width / height;

		/**
		#ifdef EditMode
				GetClientRect(hWnd, &rc);
				winW = rc.right - rc.left;
				winH = rc.bottom - rc.top;
		#else
				winW = width;
				winH = height;
		#endif
		*/

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = (int)width;
		sd.BufferDesc.Height = (int)height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = FRAMES_PER_SECOND;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = EditMode;

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};

		UINT numFeatureLevels = ARRAYSIZE(featureLevels);
		UINT flags = DebugMode ? D3D11_CREATE_DEVICE_DEBUG : 0;

		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, NULL, &g_pImmediateContext);
		
		if (FAILED(hr))
		{
			MessageBox(hWnd, "device not created", "dx11error", MB_OK);
			return S_FALSE;
		}

		ID3D11Texture2D* pBackBuffer = NULL;
		hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "swapchain error", "dx11error", MB_OK);
			return S_FALSE;
		}

		hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "rt not created", "dx11error", MB_OK);
			return S_FALSE;
		}

		pBackBuffer->Release();

/*		InitViewport();

		InitRasterizer();

		InitDepth();

		CreateConstBuf();

		SetTextureOptions();
		InitCamera();

		TextureTempMem = (char*)malloc(2048 * 2048 * 4 * 2);
		CreateTempTexture();
		*/
		return S_OK;
	}

	void Clear()
	{
		XMVECTORF32 col = { 1,0,0,1 };
		dx::g_pImmediateContext->ClearRenderTargetView(dx::g_pRenderTargetView, col);

	}

	void Present()
	{
		dx::g_pSwapChain->Present(1, 0);
	}
}
