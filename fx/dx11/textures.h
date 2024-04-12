namespace Textures
{

	#define max_tex 255
	#define mainRTIndex 0

	enum tType { flat, cube };

	DXGI_FORMAT dxTFormat[4] = { DXGI_FORMAT_R8G8B8A8_UNORM ,DXGI_FORMAT_R8G8B8A8_SNORM ,DXGI_FORMAT_R16G16B16A16_FLOAT ,DXGI_FORMAT_R32G32B32A32_FLOAT };
	enum tFormat { u8, s8, s16, s32 };
	D3D11_TEXTURE2D_DESC tdesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC svDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;

	ID3D11RenderTargetView* mrtView[8];

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

	} textureDesc;

	textureDesc texture[max_tex];

	byte currentRT = 0;

	void CreateTex(int i)
	{
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
		#if DebugMode
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
		#if DebugMode
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
				#if DebugMode
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
				#if DebugMode
				if (FAILED(hr)) { Log("CreateRenderTargetView error\n"); return; }
			#endif	
			}
		}

	}

	void Depth(int i)
	{
		auto cTex = texture[i];

		tdesc.Width = (UINT)cTex.size.x;
		tdesc.Height = (UINT)cTex.size.y;
		tdesc.MipLevels = cTex.mipMaps ? (UINT)(log2(max(cTex.size.x, cTex.size.y))) : 0;
		tdesc.CPUAccessFlags = 0;
		tdesc.SampleDesc.Count = 1;
		tdesc.SampleDesc.Quality = 0;
		tdesc.Usage = D3D11_USAGE_DEFAULT;

		tdesc.ArraySize = 1;
		tdesc.Format = DXGI_FORMAT_R32_TYPELESS;
		tdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		tdesc.MiscFlags = 0;
		HRESULT hr = device->CreateTexture2D(&tdesc, NULL, &texture[i].pDepth);
		#if DebugMode
				if (FAILED(hr)) { Log("CreateDepthStencilView error\n"); return; }
		#endif	

		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Flags = 0;

		for (unsigned int m = 0; m < max(1,tdesc.MipLevels); m++)
		{
			descDSV.Texture2D.MipSlice = m;
			HRESULT hr = device->CreateDepthStencilView(texture[i].pDepth, &descDSV, &texture[i].DepthStencilView[m]);
			#if DebugMode
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
		#if DebugMode
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
		texture[i].depth = depth;

		if (i > 0)
		{
			CreateTex(i);
			ShaderRes(i);
			rtView(i);
		}

		if (depth)
		{
			Depth(i);
			shaderResDepth(i);
		}

	}

	void UnbindAll()
	{
		ID3D11ShaderResourceView* const null[128] = { NULL };
		context->VSSetShaderResources(0, 128, null);
		context->PSSetShaderResources(0, 128, null);
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

	void CopyColor(int dst, int src)
	{
		context->CopyResource(texture[dst].pTexture, texture[src].pTexture);
	}

	void CopyDepth(int dst, int src)
	{
		context->CopyResource(texture[dst].pDepth, texture[src].pDepth);
	}

	enum tAssignType { vertex, pixel, both };

	void SetTexture(byte tex, byte slot, tAssignType tA = tAssignType::both)
	{
		if (tA == tAssignType::both || tA == tAssignType::vertex)
		{
			context->VSSetShaderResources(slot, 1, &texture[tex].TextureResView);
		}

		if (tA == tAssignType::both || tA == tAssignType::pixel)
		{
			context->PSSetShaderResources(slot, 1, &texture[tex].TextureResView);
		}
	}

	void CreateMipMap()
	{
		context->GenerateMips(texture[currentRT].TextureResView);
	}


	void SetRT(byte texId = mainRTIndex, byte level = 0)
		{
			currentRT = texId;
			
			int count = texture[texId].type == tType::cube ? 6 : 1;

			auto depthStencil = texture[texId].depth ? texture[texId].DepthStencilView[0] : 0;
			context->OMSetRenderTargets(count, &texture[texId].RenderTargetView[0][0], depthStencil);
			SetViewport(texId, level);
		}

	

	#define Texture(name,type,format,width,height,mip,depth) name,
	enum list {
		#include "..\projectFiles\texList.h"
	};

	void Init()
	{
		#undef Texture
		#define Texture(name,type,format,width,height,mip,depth) Textures::Create(list::name, Textures::tType::type,Textures::tFormat::format, XMFLOAT2((float)width,(float)height),mip,depth);
		#include "..\projectFiles\texList.h"
	}

} 