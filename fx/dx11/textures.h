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

	textureDesc Texture[max_tex];

	byte currentRT = 0;

	void CreateTex(int i)
	{
		auto cTex = Texture[i];

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

		HRESULT hr = device->CreateTexture2D(&tdesc, NULL, &Texture[i].pTexture);

		LogIfError("CreateTexture2D error");
	}

	void ShaderRes(int i)
	{
		svDesc.Format = tdesc.Format;
		svDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		if (Texture[i].type == cube)
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

		HRESULT hr = device->CreateShaderResourceView(Texture[i].pTexture, &svDesc, &Texture[i].TextureResView);
		LogIfError("CreateShaderResourceView error\n");
	}

	void rtView(int i)
	{
		renderTargetViewDesc.Format = tdesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		if (Texture[i].type == cube)
		{
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			renderTargetViewDesc.Texture2DArray.ArraySize = 1;
			renderTargetViewDesc.Texture2DArray.MipSlice = 0;

			for (int j = 0; j < 6; j++)
			{
				renderTargetViewDesc.Texture2DArray.FirstArraySlice = j;
				HRESULT hr = device->CreateRenderTargetView(Texture[i].pTexture, &renderTargetViewDesc, &Texture[i].RenderTargetView[0][j]);
				LogIfError("CreateRenderTargetView error\n");
			}
		}
		else
		{
			for (unsigned int m = 0; m < tdesc.MipLevels; m++)
			{
				renderTargetViewDesc.Texture2D.MipSlice = m;
				HRESULT hr = device->CreateRenderTargetView(Texture[i].pTexture, &renderTargetViewDesc, &Texture[i].RenderTargetView[m][0]);
				LogIfError("CreateRenderTargetView error\n");
			}
		}
	}

	void Depth(int i)
	{
		auto cTex = Texture[i];

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
		HRESULT hr = device->CreateTexture2D(&tdesc, NULL, &Texture[i].pDepth);
		#if DebugMode
				if (FAILED(hr)) { Log("CreateDepthStencilView error\n"); return; }
		#endif	

		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Flags = 0;

		for (unsigned int m = 0; m < max(1,tdesc.MipLevels); m++)
		{
			descDSV.Texture2D.MipSlice = m;
			HRESULT hr = device->CreateDepthStencilView(Texture[i].pDepth, &descDSV, &Texture[i].DepthStencilView[m]);
			LogIfError("CreateDepthStencilView error\n");
		}
	}

	void shaderResDepth(int i)
	{
		svDesc.Format = DXGI_FORMAT_R32_FLOAT;
		svDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		svDesc.Texture2D.MostDetailedMip = 0;
		svDesc.Texture2D.MipLevels = 1;

		HRESULT hr = device->CreateShaderResourceView(Texture[i].pDepth, &svDesc, &Texture[i].DepthResView);
		LogIfError("CreateShaderResourceView for Depth error\n");
	}

	void Create(int i, tType type, tFormat format, XMFLOAT2 size, bool mipMaps, bool depth)
	{
		ZeroMemory(&tdesc, sizeof(tdesc));
		ZeroMemory(&svDesc, sizeof(svDesc));
		ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
		ZeroMemory(&descDSV, sizeof(descDSV));

		Texture[i].type = type;
		Texture[i].format = format;
		Texture[i].size = size;
		Texture[i].mipMaps = mipMaps;
		Texture[i].depth = depth;

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

	void SetViewport(texture texId, byte level = 0)
	{
		XMFLOAT2 size = Textures::Texture[(int)texId].size;
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

	API CopyColor(texture dst, texture src)
	{
		context->CopyResource(Texture[(int)dst].pTexture, Texture[(int)src].pTexture);
	}

	API CopyDepth(texture dst, texture src)
	{
		context->CopyResource(Texture[(int)dst].pDepth, Texture[(int)src].pDepth);
	}

	API TextureToShader(texture tex, int slot, targetshader tA = targetshader::both)
	{
		if (tA == targetshader::both || tA == targetshader::vertex)
		{
			context->VSSetShaderResources(slot, 1, &Texture[(int)tex].TextureResView);
		}

		if (tA == targetshader::both || tA == targetshader::pixel)
		{
			context->PSSetShaderResources(slot, 1, &Texture[(int)tex].TextureResView);
		}
	}

	API CreateMipMap()
	{
		context->GenerateMips(Texture[currentRT].TextureResView);
	}


	API RenderTarget(texture texId, int level = 0)
	{
		currentRT = (int)texId;
		
		auto depthStencil = Texture[(int)texId].depth ? Texture[(int)texId].DepthStencilView[0] : 0;

		if (Texture[(int)texId].type == tType::flat)
		{
			context->OMSetRenderTargets(1, &Texture[(int)texId].RenderTargetView[0][0], depthStencil);
		}

		if (Texture[(int)texId].type == tType::cube)
		{
			context->OMSetRenderTargets(6, &Texture[(int)texId].RenderTargetView[0][0], 0);
		}

		SetViewport(texId, level);
	}

	#define CreateTexture(name,type,format,width,height,mip,depth) name,
	enum list {
		#include "..\projectFiles\texList.h"
	};

	#undef CreateTexture
	#define CreateTexture(name,type,format,width,height,mip,depth) #name,

	const char* nameList[] = {
		#include "..\projectFiles\texList.h"
	};

	void Init()
	{
		#undef CreateTexture
		#define CreateTexture(name,type,format,width,height,mip,depth) Textures::Create(list::name, Textures::tType::type,Textures::tFormat::format, XMFLOAT2((float)width,(float)height),mip,depth);
		#include "..\projectFiles\texList.h"
	}

} 