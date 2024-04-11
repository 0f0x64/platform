namespace Shaders {

	#include "shadersReflection.h"

	#if !EditMode

	#include "..\generated\processedShaders.h"
	#include "..\generated\libList.h"

	#endif

	int vsCount = 0;
	int psCount = 0;


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

		void CreateVS(int i, const char* name)
		{
			LPCWSTR source = nameToPatchLPCWSTR(name);

			HRESULT hr = S_OK;

			hr = D3DCompileFromFile(source, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_4_1", NULL, NULL, &VS[i].pBlob, &pErrorBlob);
			CompilerLog(source, hr, "vertex shader compiled: ");

			if (hr == S_OK)
			{
				hr = device->CreateVertexShader(VS[i].pBlob->GetBufferPointer(), VS[i].pBlob->GetBufferSize(), NULL, &VS[i].pShader);

				#if DebugMode
					if (FAILED(hr)) { Log("vs creation fail\n"); return; }
				#endif
			}

		}

		void CreatePS(int i, const char* name)
		{
			LPCWSTR source = nameToPatchLPCWSTR(name);

			HRESULT hr = S_OK;

			hr = D3DCompileFromFile(source, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_4_1", NULL, NULL, &PS[i].pBlob, &pErrorBlob);
			CompilerLog(source, hr, "pixel shader compiled: ");

			if (hr == S_OK)
			{
				hr = device->CreatePixelShader(PS[i].pBlob->GetBufferPointer(), PS[i].pBlob->GetBufferSize(), NULL, &PS[i].pShader);
				#if DebugMode
					if (FAILED(hr)) { Log("vs creation fail\n"); return; }
				#endif
			}

		}

		void CreateShaders()
		{
			vsCount = sizeof(vsList) / sizeof(const char*);
			int i = 0;
			while (i < vsCount)
			{
				char fileName[255];
				strcpy(fileName, "/vs/");
				strcat(fileName, vsList[i]);
				strcat(fileName, shaderExtension);
				CreateVS(i, fileName);
				i++;

			}

			psCount = sizeof(psList) / sizeof(const char*);
			i = 0;
			while (i < psCount)
			{
				char fileName[255];
				strcpy(fileName, "/ps/");
				strcat(fileName, psList[i]);
				strcat(fileName, shaderExtension);
				CreatePS(i, fileName);
				i++;
			}

		}

#else



		int getPtrIndex(const char* shaderName)
		{
			int j = 0; while (strcmp(libName[j], shaderName)) j++;
			return j;
		}

		char* outText;
		//---------
		char* processIncludes(const char* shaderText)
		{
			if (!outText) outText = new char[64000];
			strcpy(outText, shaderText);
			char* inc = outText;

			while (true)
			{
				inc = strstr(inc, "#include");
				if (inc == NULL) break; 

				char* b1 = strstr(inc, "lib/") + 4;
				char* b2 = strstr(b1, ".");
				char* incEnd = strstr(b2, ">");
				char name[255];
				int sz = b2 - b1;
				memcpy(name, b1, sz);
				name[sz] = 0;
				int aa = strlen(name);

				int index = getPtrIndex(name);
				const char* includePtr = libPtr[index];
				int includeSize = strlen(libPtr[index]);

				int textSize = strlen(outText);
				memmove(inc + includeSize-1, incEnd + 1, textSize - (incEnd - outText)+1);
				memcpy(inc, includePtr, includeSize);

			}

			return outText;
		}

		void CreateVS(int n, const char* shaderText)
		{
			HRESULT hr = S_OK;

			VS[n].pBlob = NULL;
			auto ptr = processIncludes(shaderText);
			hr = D3DCompile(ptr, strlen(ptr), NULL, NULL, NULL, "VS", "vs_4_1", NULL, NULL, &VS[n].pBlob, &pErrorBlob);

#if DebugMode
			if (FAILED(hr)) {
				auto m = (char*)pErrorBlob->GetBufferPointer();
				Log(m);
			}
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


		void CreatePS(int n, const char* shaderText)
		{
			HRESULT hr = S_OK;

			PS[n].pBlob = NULL;
			auto ptr = processIncludes(shaderText);
			hr = D3DCompile(ptr, strlen(ptr), NULL, NULL, NULL, "PS", "ps_4_1", NULL, NULL, &PS[n].pBlob, &pErrorBlob);

#if DebugMode
			if (FAILED(hr)) {
				auto m = (char*)pErrorBlob->GetBufferPointer();
				Log(m);
			}
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

		#include "..\generated\processedShadersCompile.h"

		void CreateShaders()
		{
			CompileAll();
		}

	#endif

	void Init()
	{
		CreateShaders();
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