void paramConstBufInit();

namespace ConstBuf
{
	ID3D11Buffer* buffer[5];
	
	enum class cBuffer { reserved, camera , frame, global, extra };

	#define constCount 32

	//b1 
	struct {
		XMMATRIX world[2];
		XMMATRIX view[2];
		XMMATRIX proj[2];
	} camera;//update per camera set

	//b2
	struct {
		XMFLOAT4 time;
		XMFLOAT4 aspect;
	} frame;//update per frame

	//b3
	XMFLOAT4 global[constCount];//update once on start

	//b4
	XMFLOAT4 extra[4095];
	


	char* cBufPtr[] = { NULL, (char*)&camera ,(char*)&frame,(char*)&global};

	int roundUp(int n, int r)
	{
		return 	n - (n % r) + r;
	}

	void Create(ID3D11Buffer* &buf, int size)
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = roundUp(size, 16);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.StructureByteStride = 16;

		HRESULT hr = device->CreateBuffer(&bd, NULL, &buf);
		LogIfError("constant buffer fail\n");
	
	
	}

	//custom structured buffer
	//todo reflect all CB and SB
	
#if EditMode
	struct vertex  {
		float4 position;
	};

	struct index  {
		float4 index;
	};

	ID3D11Buffer* pSBuffer[2];
	ID3D11ShaderResourceView* pSB_SRV[2];

	void CreateSB(int slot,int size,int count, auto& data)
	{

		if (pSBuffer[slot]) { pSBuffer[slot]->Release(); pSBuffer[slot] = nullptr; }
		if (pSB_SRV[slot]) { pSB_SRV[slot]->Release(); pSB_SRV[slot] = nullptr; }


		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = data;

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = size*count ; 
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Для чтения в шейдере
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = size; // Обязательно для Structured Buffer

		HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &pSBuffer[slot]);

		// 2. Создаем Shader Resource View (SRV)
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Всегда UNKNOWN для Structured Buffer
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = count;

		HRESULT hr2 = device->CreateShaderResourceView(pSBuffer[slot], &srvDesc, &pSB_SRV[slot]);
	}

	void BindSB(int slot)
	{
		context->VSSetShaderResources(slot, 1, &pSB_SRV[slot]);
	}

	vertex* vArray;
	//[] = {
	//#include "../projectFiles/girl_mid_vert.h"
	//};

	index* iArray; 
	//[] = {
//		#include "../projectFiles/girl_mid_ind.h"
	//};

	uint32_t vertexCount = 0;
	uint32_t triangleCount = 0;

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>


	bool LoadObjToPointers(const std::string& filename, vertex** outVertices, index** outIndices, uint32_t& vCount, uint32_t& iCount) 
	{
		std::vector<float4> temp_positions;
		std::vector<vertex> final_vertices;
		std::vector<index> final_triangles;

		std::ifstream file(filename);
		if (!file.is_open())
		{
			return false;
		}

		std::string line;
		while (std::getline(file, line)) {
			if (line.empty()) continue;
			std::stringstream ss(line);
			std::string prefix;
			ss >> prefix;

			if (prefix == "v") {
				float4 p;
				ss >> p.x >> p.y >> p.z;
				p.w = 1.0f;
				temp_positions.push_back(p);
			}
			else if (prefix == "f") {
				index tri = {};
				// We assume a triangulated OBJ (3 vertices per face)
				for (int i = 0; i < 3; i++) {
					std::string vStr;
					ss >> vStr;

					// Parse indices (v/vt/vn). We only care about 'v' (pIdx)
					int pIdx = 0, uIdx = 0, nIdx = 0;
					if (sscanf_s(vStr.c_str(), "%d/%d/%d", &pIdx, &uIdx, &nIdx) >= 1) {
						vertex v;
						v.position = temp_positions[pIdx - 1];

						// Create a unique vertex for this face corner
						uint32_t currentVIdx = (uint32_t)final_vertices.size();
						final_vertices.push_back(v);

						// Store index in the float4 fields
						if (i == 0)      tri.index.x = (float)currentVIdx;
						else if (i == 1) tri.index.y = (float)currentVIdx;
						else if (i == 2) tri.index.z = (float)currentVIdx;
					}
				}
				tri.index.w = 0.0f;
				final_triangles.push_back(tri);
			}
		}

		vCount = (uint32_t)final_vertices.size();
		iCount = (uint32_t)final_triangles.size();

		if (vCount == 0) return false;

		// Allocate heap memory
		if (*outVertices)
		{
			delete[] *outVertices;
			*outVertices = nullptr;
		}
		if (*outIndices)
		{
			delete[] *outIndices;
			*outIndices = nullptr;
		}

		*outVertices = new vertex[vCount];
		*outIndices = new index[iCount];

		std::memcpy(*outVertices, final_vertices.data(), sizeof(vertex) * vCount);
		std::memcpy(*outIndices, final_triangles.data(), sizeof(index) * iCount);

		return true;
	}

	void LoadObj(const char* name)
	{
		if (LoadObjToPointers(name, &vArray, &iArray, vertexCount, triangleCount))
		{
			float xMax = 0;
			float xMin = 0;
			float yMax = 0;
			float yMin = 0;
			float zMax = 0;
			float zMin = 0;

			for (int i = 0; i < vertexCount; i++)
			{
				xMax = max(xMax, vArray[i].position.x);
				xMin = min(xMin, vArray[i].position.x);

				yMax = max(yMax, vArray[i].position.y);
				yMin = min(yMin, vArray[i].position.y);

				zMax = max(zMax, vArray[i].position.z);
				zMin = min(zMin, vArray[i].position.z);

			}
			float xCenter = (xMax + xMin) / 2.;
			float yCenter = (yMax + yMin) / 2.;
			float zCenter = (zMax + zMin) / 2.;
			float xSize = (yMax - yMin);
			float ySize = (yMax - yMin);
			float zSize = (zMax - zMin);

			for (int i = 0; i < vertexCount; i++)
			{
				vArray[i].position.x -= xCenter;
				vArray[i].position.y -= yCenter;
				vArray[i].position.z -= zCenter;

				vArray[i].position.x *= 4/ySize;
				vArray[i].position.y *= 4/ySize;
				vArray[i].position.z *= 4/ySize;

			}


			CreateSB(0, sizeof(vertex), vertexCount, vArray);
			CreateSB(1, sizeof(index), triangleCount, iArray);
		}
	}

#endif

	void Init()
	{
		paramConstBufInit();

		Create(buffer[(int)cBuffer::camera], sizeof(camera));
		Create(buffer[(int)cBuffer::frame], sizeof(frame));
		Create(buffer[(int)cBuffer::global], sizeof(global));
		Create(buffer[(int)cBuffer::extra], sizeof(extra));

#if EditMode
		LoadObjToPointers("projectFiles//hero.obj", &vArray, &iArray, vertexCount, triangleCount);

		CreateSB(0, sizeof(vertex), vertexCount, vArray);
		CreateSB(1, sizeof(index), triangleCount, iArray);
#endif
	}

	void Update(cBuffer i)
	{
		context->UpdateSubresource(buffer[(int)i], 0, NULL, cBufPtr[(int)i], 0, 0);
	}

	void Update(ID3D11Buffer* buf, auto& data)
	{
		context->UpdateSubresource(buf, 0, NULL, &data, 0, 0);
	}

	enum class target {vertex,pixel,both};

	void Set(cBuffer i, target shader)
	{
		if (shader == target::vertex || shader == target::both)
		{
			context->VSSetConstantBuffers((int)i, 1, &buffer[(int)i]);
		}

		if (shader == target::pixel || shader == target::both)
		{
			context->PSSetConstantBuffers((int)i, 1, &buffer[(int)i]);
		}

	}


	
	
} 

