using namespace DirectX;

namespace dx
{

	#if DebugMode
		void Log(const char* message)
		{
			#if EditMode
				OutputDebugString(message);
			#else	
				MessageBox(hWnd, message, "", MB_OK);
			#endif	
		}
	#endif

	ID3D11Device* device = NULL;
	ID3D11DeviceContext* context = NULL;
	IDXGISwapChain* swapChain = NULL;
	ID3D11RenderTargetView* renderTargetView = NULL;

	int width;
	int height;
	float aspect;
	float iaspect;

	namespace Camera
	{
		XMMATRIX View;
		XMMATRIX Projection;
		XMMATRIX DepthView;
		XMMATRIX DepthProjection;

		void Set()
		{
			View = XMMatrixTranslation(0, 0, 1);
			DepthView = View;
			Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);
			DepthProjection = Projection;
		}
	}

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
			frame.time = XMFLOAT4((float)(timer::frameBeginTime*.01), 0, 0, 0);
			context->UpdateSubresource(vertex, 0, NULL, &frame, 0, 0);
			context->UpdateSubresource(pixel, 0, NULL, &frame, 0, 0);
		}

		void Set()
		{
			context->VSSetConstantBuffers(0, 1, &vertex);
			context->PSSetConstantBuffers(0, 1, &pixel);
		}
	}

	namespace Viewport {

		D3D11_VIEWPORT vp;

		void Set()
		{
			vp.Width = (FLOAT)width;
			vp.Height = (FLOAT)height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			context->RSSetViewports(1, &vp);
		}
	};

	namespace Blend
	{

		ID3D11BlendState* blendState[4][5];

		void Init()
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
	}

	namespace Rasterizer {

		ID3D11RasterizerState* rasterState[3];

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
			rasterizerState.AntialiasedLineEnable = true;
			device->CreateRasterizerState(&rasterizerState, &rasterState[0]);

			rasterizerState.CullMode = D3D11_CULL_FRONT;
			device->CreateRasterizerState(&rasterizerState, &rasterState[1]);

			rasterizerState.CullMode = D3D11_CULL_BACK;
			device->CreateRasterizerState(&rasterizerState, &rasterState[2]);

			context->RSSetState(rasterState[0]);
		}
	}

	namespace Depth {

		ID3D11Texture2D* depthStencil = NULL;
		ID3D11DepthStencilView* depthStencilView = NULL;
		ID3D11ShaderResourceView* depthStencilSRView = NULL;
		ID3D11DepthStencilState* pDSState[4];

		void Init()
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

		void Mode()
		{
			context->OMSetDepthStencilState(dx::Depth::pDSState[0], 1);
		}
	}

	namespace Textures {

		#define max_tex 255

		ID3D11SamplerState* pSampler[13];

		void InitSampler()
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

		enum tType { flat, cube };

		DXGI_FORMAT dxTFormat[4] = { DXGI_FORMAT_R8G8B8A8_UNORM ,DXGI_FORMAT_R8G8B8A8_SNORM ,DXGI_FORMAT_R16G16B16A16_FLOAT ,DXGI_FORMAT_R32G32B32A32_FLOAT };
		enum tFormat { u8, s8, s16, s32 };
		D3D11_TEXTURE2D_DESC tdesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC svDesc;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;

		typedef struct {
			ID3D11Texture2D* pTexture;
			ID3D11ShaderResourceView* TextureResView;
			ID3D11RenderTargetView* RenderTargetView[16][6];//16 for possible mips? // 6 for cubemap

			ID3D11Texture2D* pDepth;
			ID3D11ShaderResourceView* DepthResView;
			ID3D11DepthStencilView* DepthStencilView[16];

			tType type;
			tFormat format;
			XMFLOAT2 size;
			bool mipMaps;
			bool depth;
	
		} tex;



		tex texture[max_tex];

		byte currentRT = 0;


		void CreateTex(int i)
		{
			//if (texture[i].pTexture) texture[i].pTexture->Release();
			//if (texture[i].TextureResView) texture[i].TextureResView->Release();
			auto cTex = texture[i];

			tdesc.Width = (UINT)cTex.size.x;
			tdesc.Height = (UINT)cTex.size.y;
			tdesc.MipLevels = cTex.mipMaps ? (UINT)(log2(max(cTex.size.x, cTex.size.y))) : 0;
			tdesc.ArraySize = 1;
			tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			tdesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			tdesc.CPUAccessFlags = 0;
			tdesc.SampleDesc.Count = 1;
			tdesc.SampleDesc.Quality = 0;
			tdesc.Usage = D3D11_USAGE_DEFAULT;
			tdesc.Format = dxTFormat[cTex.format];

			if (cTex.type == cube)
			{
				tdesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | tdesc.MiscFlags;
				tdesc.ArraySize = 6;
			}

			HRESULT hr = device->CreateTexture2D(&tdesc, NULL, &texture[i].pTexture);
#ifdef DebugMode
			if (FAILED(hr)) { Log("CreateTexture2D error\n"); return; }
#endif	
		}

		void ShaderRes(int i)
		{
			svDesc.Format = tdesc.Format;
			svDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

			if (texture[i].type == cube)
			{
				svDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
				svDesc.TextureCube.MostDetailedMip = 0;
				svDesc.TextureCube.MipLevels = -1;

			}
			else
			{
				svDesc.Texture2D.MipLevels = -1;
				svDesc.Texture2D.MostDetailedMip = 0;
			}

			HRESULT hr = device->CreateShaderResourceView(texture[i].pTexture, &svDesc, &texture[i].TextureResView);
#ifdef DebugMode
			if (FAILED(hr)) { Log("CreateShaderResourceView error\n"); return; }
#endif	
		}

		void rtView(int i)
		{
			renderTargetViewDesc.Format = tdesc.Format;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;

			if (texture[i].type == cube)
			{
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				renderTargetViewDesc.Texture2DArray.ArraySize = 1;
				renderTargetViewDesc.Texture2DArray.MipSlice = 0;

				for (int j = 0; j < 6; j++)
				{
					renderTargetViewDesc.Texture2DArray.FirstArraySlice = j;
					HRESULT hr = device->CreateRenderTargetView(texture[i].pTexture, &renderTargetViewDesc, &texture[i].RenderTargetView[0][j]);
#ifdef DebugMode
					if (FAILED(hr)) { Log("CreateRenderTargetView error\n"); return; }
#endif	
				}
			}
			else
			{
				for (unsigned int m = 0; m < tdesc.MipLevels; m++)
				{
					renderTargetViewDesc.Texture2D.MipSlice = m;
					HRESULT hr = device->CreateRenderTargetView(texture[i].pTexture, &renderTargetViewDesc, &texture[i].RenderTargetView[m][0]);
#ifdef DebugMode
					if (FAILED(hr)) { Log("CreateRenderTargetView error\n"); return; }
#endif	
				}
			}

		}

		void Depth(int i)
		{
			tdesc.ArraySize = 1;
			tdesc.Format = DXGI_FORMAT_R32_TYPELESS;
			tdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			tdesc.MiscFlags = 0;
			HRESULT hr = device->CreateTexture2D(&tdesc, NULL, &texture[i].pDepth);
#ifdef DebugMode
			if (FAILED(hr)) { Log("CreateDepthStencilView error\n"); return; }
#endif	

			descDSV.Format = DXGI_FORMAT_D32_FLOAT;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Flags = 0;

			for (unsigned int m = 0; m < tdesc.MipLevels; m++)
			{
				descDSV.Texture2D.MipSlice = m;
				HRESULT hr = device->CreateDepthStencilView(texture[i].pDepth, &descDSV, &texture[i].DepthStencilView[m]);
#ifdef DebugMode
				if (FAILED(hr)) { Log("CreateDepthStencilView error\n"); return; }
#endif			

			}

		}

		void shaderResDepth(int i)
		{
			svDesc.Format = DXGI_FORMAT_R32_FLOAT;
			svDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			svDesc.Texture2D.MostDetailedMip = 0;
			svDesc.Texture2D.MipLevels = 1;

			HRESULT hr = device->CreateShaderResourceView(texture[i].pDepth, &svDesc, &texture[i].DepthResView);
#ifdef DebugMode
			if (FAILED(hr)) { Log("CreateShaderResourceView for Depth error\n"); return; }
#endif			
		}

		void Create(int i, tType type, tFormat format, XMFLOAT2 size, bool mipMaps, bool depth)
		{
			ZeroMemory(&tdesc, sizeof(tdesc));
			ZeroMemory(&svDesc, sizeof(svDesc));
			ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
			ZeroMemory(&descDSV, sizeof(descDSV));

			texture[i].type = type;
			texture[i].format = format;
			texture[i].size = size;
			texture[i].mipMaps = mipMaps;
			texture[i].depth  = depth;

			CreateTex(i);
			ShaderRes(i);
			rtView(i);

			if (depth)
			{
				Depth(i);
				shaderResDepth(i);
			}

		}

		void UnbindAll()
		{
			ID3D11ShaderResourceView* const null[128] = { NULL };
			context->PSSetShaderResources(0, 128, null);
		}

		void CreateMipMap(int i)
		{
			context->GenerateMips(texture[i].TextureResView);
		}

		void SetViewport(byte texId, byte level = 0)
		{
			XMFLOAT2 size = Textures::texture[texId].size;
			float denom = powf(2, level);
			
			D3D11_VIEWPORT vp;

			vp.Width = (FLOAT)size.x / denom;
			vp.Height = (FLOAT)size.y / denom;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;

			context->RSSetViewports(1, &vp);
		}

		enum tAssignType{vertex,pixel,both};

		void Set(byte tex, byte slot, tAssignType tA = tAssignType::both)
		{
			if (tA == tAssignType::both || tA == tAssignType::vertex)
			{
//				context->VSSetShaderResources(slot, 1, &texture[tex].TextureResView);
	//			context->VSSetSamplers(slot, 1, &pSampler[0]);
			}

			if (tA == tAssignType::both || tA == tAssignType::pixel)
			{
				context->PSSetShaderResources(slot, 1, &texture[tex].TextureResView);
				context->PSSetSamplers(slot, 1, &pSampler[0]);
			}
		}

		namespace RenderTargets {

			const byte mainRT = max_tex;

			void Set(byte texId = mainRT, byte level = 0)
			{
				currentRT = texId;

				if (texId == mainRT)
				{
					Viewport::Set();
					context->OMSetRenderTargets(1, &renderTargetView, Depth::depthStencilView);
				}
				else
				{
					Textures::SetViewport(texId, level);
					context->OMSetRenderTargets(1, &Textures::texture[texId].RenderTargetView[0][0], Textures::texture[texId].depth ? Textures::texture[texId].DepthStencilView[0] : 0);
				}
			}

		};

		void Init()
		{
			InitSampler();
		}
}

	namespace Device {

		D3D_DRIVER_TYPE	driverType = D3D_DRIVER_TYPE_NULL;

		void Init()
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
				if (FAILED(hr)) { Log("device not created\n"); return; }
			#endif	

			ID3D11Texture2D* pBackBuffer = NULL;
			hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			#if DebugMode
				if (FAILED(hr)) { Log("swapchain error\n"); return; }
			#endif		

			hr = device->CreateRenderTargetView(pBackBuffer, NULL, &Textures::texture[0].RenderTargetView[0][0]);
			#if DebugMode
				if (FAILED(hr)) { Log("rt not created\n"); return; }
			#endif	

			pBackBuffer->Release();

		}
	}

	namespace Shaders {

		int vsCount = 0;
		int psCount = 0;

		#include "shadersReflection.h"

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

		namespace Compiler {

			#if EditMode

			void CompilerLog(LPCWSTR source, HRESULT hr, const char* message)
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

			void Vertex(int i, const char* name)
			{
				LPCWSTR source = nameToPatchLPCWSTR(name);

				HRESULT hr = S_OK;
				
				hr = D3DCompileFromFile(source, NULL, NULL, "VS", "vs_4_1", NULL, NULL, &VS[i].pBlob, &pErrorBlob);
				CompilerLog(source, hr, "vertex shader compiled: ");

				if (hr == S_OK)
				{
					hr = device->CreateVertexShader(VS[i].pBlob->GetBufferPointer(), VS[i].pBlob->GetBufferSize(), NULL, &VS[i].pShader);
					#if DebugMode
						if (FAILED(hr)) { Log("vs creation fail\n"); return; }
					#endif
				}

			}

			void Pixel(int i, const char* name)
			{
				LPCWSTR source = nameToPatchLPCWSTR(name);

				HRESULT hr = S_OK;

				hr = D3DCompileFromFile(source, NULL, NULL, "PS", "ps_4_1", NULL, NULL, &PS[i].pBlob, &pErrorBlob);
				CompilerLog(source, hr, "pixel shader compiled: ");

				if (hr == S_OK)
				{
					hr = device->CreatePixelShader(PS[i].pBlob->GetBufferPointer(), PS[i].pBlob->GetBufferSize(), NULL, &PS[i].pShader);
					#if DebugMode
						if (FAILED(hr)) { Log("vs creation fail\n"); return; }
					#endif
				}

			}

			#else

			void Vertex(int n, const char* shaderText)
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


			void Pixel(int n, const char* shaderText)
			{
				HRESULT hr = S_OK;

				PS[n].pBlob = NULL;
				hr = D3DCompile(shaderText, strlen(shaderText), NULL, NULL, NULL, "PS", "ps_4_1", D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR, NULL, &PS[n].pBlob, &pErrorBlob);

				#if DebugMode
				if (hr != S_OK) { Log("vs fail"); }
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

		void Set(int v, int p)
		{
			SetVS(v);
			SetPS(p);
		}
	}

	void Init()
	{
		Device::Init();
		Textures::Init();
		Rasterizer::Init();
		Depth::Init();
		Blend::Init();
		Viewport::Set();
		Camera::Set();
		ConstBuf::Init();
	}

	namespace Draw
	{
		void Clear(float r, float g, float b, float a)
		{
			context->ClearRenderTargetView(Textures::texture[Textures::currentRT].RenderTargetView[0][0], XMVECTORF32{ r,g,b,a });
		}


		void NullDrawer(int instances, int quadCount)
		{
			context->DrawInstanced(quadCount * 6, instances, 0, 0);
		}
	}

	void Present()
	{
		swapChain->Present(1, 0);
	}

	void SetIA()
	{
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(NULL);
		context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	}

}
