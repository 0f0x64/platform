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

		const char* shaderExtension = ".shader";

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

	#if EditMode

		void CreateShaders()
		{
			Shaders::vsCount = sizeof(Shaders::vsList) / sizeof(const char*);
			int i = 0;
			while (i < Shaders::vsCount)
			{
				char fileName[255];
				strcpy(fileName, "/vs/");
				strcat(fileName, Shaders::vsList[i]);
				strcat(fileName, shaderExtension);
				dx::Shaders::Compiler::Vertex(i, fileName);
				i++;

			}

			Shaders::psCount = sizeof(Shaders::psList) / sizeof(const char*);
			i = 0;
			while (i < Shaders::psCount)
			{
				char fileName[255];
				strcpy(fileName, "/ps/");
				strcat(fileName, Shaders::psList[i]);
				strcat(fileName, shaderExtension);
				dx::Shaders::Compiler::Pixel(i, fileName);
				i++;
			}

		}

	#else

	#include "generated\processedShaders.h"

		void CreateShaders()
		{
			shadersData::CompileAll();
		}

	#endif

	}

	void Init()
	{
		Compiler::CreateShaders();
	}

	//todo: check previously setted shader, same for IA, const, etc
	struct {
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
	} Api;
}