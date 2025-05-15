namespace Device
{

	D3D_DRIVER_TYPE	driverType = D3D_DRIVER_TYPE_NULL;

	void Init()
	{
		HRESULT hr;

		aspect = float(height) / float(width);
		iaspect = float(width) / float(height);

		DXGI_SWAP_CHAIN_DESC sd= {

				.BufferDesc = {
					.Width = (UINT)width,
					.Height = (UINT)height,
					.RefreshRate = {
						.Numerator = FRAMES_PER_SECOND,
						.Denominator = 1
						},
					.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
					.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
					.Scaling = DXGI_MODE_SCALING_UNSPECIFIED
				},

				.SampleDesc = {
					.Count = 1,
					.Quality = 0
				},

				.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
				.BufferCount = 2,
				.OutputWindow = hWnd,
				.Windowed = EditMode,
				.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
				.Flags = 0
		};

		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, DirectXDebugMode ? D3D11_CREATE_DEVICE_DEBUG : 0, 0, 0, D3D11_SDK_VERSION, &sd, &swapChain, &device, NULL, &context);
		LogIfError("device not created\n");

		Textures::Texture[0].pTexture = NULL;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&Textures::Texture[0].pTexture);
		LogIfError("swapchain error\n");

		hr = device->CreateRenderTargetView(Textures::Texture[0].pTexture, NULL, &Textures::Texture[0].RenderTargetView[0][0]);
		LogIfError("rt not created\n");

		Textures::Texture[0].pTexture->Release();

	}

}