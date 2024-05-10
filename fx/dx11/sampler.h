namespace Sampler
{

	ID3D11SamplerState* pSampler[3][2][2];//filter, addressU, addressV
	ID3D11SamplerState* pSamplerComp;//for shadowmapping

	void Init()
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));

		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		D3D11_FILTER filter[] = { D3D11_FILTER_MIN_MAG_MIP_LINEAR,
								  D3D11_FILTER_MIN_MAG_MIP_POINT,
								  D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR };

		D3D11_TEXTURE_ADDRESS_MODE address[] = { D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_WRAP };

		constexpr byte fc = sizeof(filter) / sizeof(D3D11_FILTER);
		constexpr byte ac = sizeof(address) / sizeof(D3D11_TEXTURE_ADDRESS_MODE);

		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		for (byte f = 0; f < fc; f++)
		{
			sampDesc.Filter = (D3D11_FILTER)filter[f];

			for (byte u = 0; u < ac; u++)
			{
				for (byte v = 0; v < ac; v++)
				{
					sampDesc.AddressU = address[u];
					sampDesc.AddressV = address[v];
					HRESULT hr = device->CreateSamplerState(&sampDesc, &pSampler[f][u][v]);
					LogIfError("CreateSamplerStatw error\n");
				}
			}
		}

		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		for (int x = 0; x < 4; x++) sampDesc.BorderColor[x] = 0;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		device->CreateSamplerState(&sampDesc, &pSamplerComp);

	}

	API Sampler(targetshader shader, int slot, filter filterType, addr addressU, addr addressV)
	{
		if (shader == targetshader::vertex) context->VSSetSamplers(slot, 1, &pSampler[(int)filterType][(int)addressU][(int)addressV]);
		if (shader == targetshader::pixel) context->PSSetSamplers(slot, 1, &pSampler[(int)filterType][(int)addressU][(int)addressV]);
	}

	API SamplerComp(int slot)
	{
		context->PSSetSamplers(slot, 1, &pSamplerComp);
	}



}