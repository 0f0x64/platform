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

	struct sbObject{

		struct vertex {
			float4 position;
		};

		struct index {
			float4 index;
		};

		vertex* vArray;
		index* iArray;

		uint32_t vertexCount = 0;
		uint32_t triangleCount = 0;

		ID3D11Buffer* pSBuffer[2];
		ID3D11ShaderResourceView* pSB_SRV[2];

		void CreateSB(int slot, int size, int count, auto& data)
		{

			if (pSBuffer[slot]) { pSBuffer[slot]->Release(); pSBuffer[slot] = nullptr; }
			if (pSB_SRV[slot]) { pSB_SRV[slot]->Release(); pSB_SRV[slot] = nullptr; }


			D3D11_SUBRESOURCE_DATA initData = {};
			initData.pSysMem = data;

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = size * count;
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
				delete[] * outVertices;
				*outVertices = nullptr;
			}
			if (*outIndices)
			{
				delete[] * outIndices;
				*outIndices = nullptr;
			}

			*outVertices = new vertex[vCount];
			*outIndices = new index[iCount];

			std::memcpy(*outVertices, final_vertices.data(), sizeof(vertex) * vCount);
			std::memcpy(*outIndices, final_triangles.data(), sizeof(index) * iCount);

			return true;
		}

		bool loaded = false;

		void Load(const char* name)
		{
			if (loaded) return;

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

					vArray[i].position.x *= 4 / ySize;
					vArray[i].position.y *= 4 / ySize;
					vArray[i].position.z *= 4 / ySize;

				}


				CreateSB(0, sizeof(vertex), vertexCount, vArray);
				CreateSB(1, sizeof(index), triangleCount, iArray);

				loaded = true;
			}
		}

	};
	/*
	struct vertex  {
		float4 position;
		XMUINT4 joints;
		XMFLOAT4 weights;
	};

	struct index  {
		float4 index;
	};

	vertex* vArray;
	index* iArray;

	uint32_t vertexCount = 0;
	uint32_t triangleCount = 0;


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
	*/



#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>


#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#include "gltfAnimation.h"


	//bool LoadObjToPointersGLTF(const std::string& filename, vertex** outVertices, index** outIndices, uint32_t& vCount, uint32_t& iCount) 
	//{
	//	cgltf_options options = {0};
	//	cgltf_data* data = NULL;
	//	cgltf_result result = cgltf_parse_file(&options, filename.c_str(), &data);
	//	if (result == cgltf_result_success)
	//	{

	//		result = cgltf_load_buffers(&options, data, filename.c_str());
	//		if (result != cgltf_result_success) {
	//			return false;
	//		}

	//		gltfAnim::ReadSkeleton(data);
	//		gltfAnim::ReadAnimations(data);

	//		vCount = 0;
	//		iCount = 0;

	//		for (size_t i = 0; i < data->meshes_count; ++i) {
	//			cgltf_mesh* mesh = &data->meshes[i];

	//			for (size_t j = 0; j < mesh->primitives_count; ++j) {
	//				cgltf_primitive* prim = &mesh->primitives[j];

	//				// Ищем атрибут позиции для подсчета вершин
	//				for (size_t k = 0; k < prim->attributes_count; ++k) {
	//					if (prim->attributes[k].type == cgltf_attribute_type_position) {
	//						vCount += prim->attributes[k].data->count;
	//						break;
	//					}
	//				}
	//				// Считаем индексы
	//				if (prim->indices) {
	//					iCount += (uint32_t)(prim->indices->count / 3);
	//				}
	//				else {
	//					for (size_t k = 0; k < prim->attributes_count; ++k) {
	//						if (prim->attributes[k].type == cgltf_attribute_type_position) {
	//							iCount += (uint32_t)(prim->attributes[k].data->count / 3);
	//							break;
	//						}
	//					}
	//				}
	//			}
	//		}

	//		
	//		// 2. Выделение памяти по вашему шаблону
	//		if (*outVertices) {
	//			delete[] * outVertices;
	//			*outVertices = nullptr;
	//		}
	//		if (*outIndices) {
	//			delete[] * outIndices;
	//			*outIndices = nullptr;
	//		}

	//		if (vCount == 0) return false;

	//		*outVertices = new vertex[vCount];
	//		if (iCount > 0) {
	//			*outIndices = new index[iCount];
	//		}

	//		// 3. Второй проход: заполнение массивов данные
	//		
	//		size_t vertexOffset = 0;
	//		size_t indexOffset = 0;

	//		for (size_t i = 0; i < data->meshes_count; ++i) 
	//		{
	//			cgltf_mesh* mesh = &data->meshes[i];

	//			for (size_t j = 0; j < mesh->primitives_count; ++j) {
	//				cgltf_primitive* prim = &mesh->primitives[j];
	//				size_t prim_vertex_count = 0;



	//				// --- Чтение вершин (Позиции) ---
	//				for (size_t k = 0; k < prim->attributes_count; ++k) {
	//					if (prim->attributes[k].type == cgltf_attribute_type_position) {
	//						cgltf_accessor* acc = prim->attributes[k].data;
	//						prim_vertex_count = acc->count;

	//						for (size_t v = 0; v < prim_vertex_count; ++v) {
	//							gltfAnim::FillSkinDefaults((*outVertices)[vertexOffset + v]);
	//							float position_element[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	//							// Безопасная функция cgltf сама учитывает stride, offset, sparse данные и тип компонента
	//							if (cgltf_accessor_read_float(acc, v, position_element, 4)) {
	//								(*outVertices)[vertexOffset + v].position = float4{
	//									position_element[0],
	//									position_element[1],
	//									position_element[2],
	//									(float)i
	//								};
	//							}
	//							else {
	//								// Если не удалось прочитать, пишем нули во избежание мусора
	//								(*outVertices)[vertexOffset + v].position = float4{ 0.0f, 0.0f, 0.0f, 1.0f };
	//							}
	//						}
	//						break;
	//					}
	//				}

	//				// --- Skinning data ---
	//				for (size_t k = 0; k < prim->attributes_count; ++k) {
	//					cgltf_attribute* attr = &prim->attributes[k];
	//					if (attr->type == cgltf_attribute_type_joints) {
	//						cgltf_accessor* acc = attr->data;
	//						for (size_t v = 0; v < acc->count && v < prim_vertex_count; ++v) {
	//							cgltf_uint joints[4] = {0, 0, 0, 0};
	//							if (cgltf_accessor_read_uint(acc, v, joints, 4)) {
	//								XMUINT4 mapped(0, 0, 0, 0);
	//								for (int c = 0; c < 4; ++c) {
	//									uint32_t jointIndex = joints[c];
	//									if (data->skins_count > 0 && jointIndex < data->skins[0].joints_count) {
	//										jointIndex = gltfAnim::NodeIndex(data, data->skins[0].joints[jointIndex]);
	//									}
	//									if (c == 0) mapped.x = jointIndex; else if (c == 1) mapped.y = jointIndex; else if (c == 2) mapped.z = jointIndex; else mapped.w = jointIndex;
	//								}
	//								(*outVertices)[vertexOffset + v].joints = mapped;
	//							}
	//						}
	//					}
	//					else if (attr->type == cgltf_attribute_type_weights) {
	//						cgltf_accessor* acc = attr->data;
	//						for (size_t v = 0; v < acc->count && v < prim_vertex_count; ++v) {
	//							float weights[4] = {1.0f, 0.0f, 0.0f, 0.0f};
	//							if (cgltf_accessor_read_float(acc, v, weights, 4)) {
	//								float sum = weights[0] + weights[1] + weights[2] + weights[3];
	//								if (sum > 0.000001f) {
	//									weights[0] /= sum;
	//									weights[1] /= sum;
	//									weights[2] /= sum;
	//									weights[3] /= sum;
	//								}
	//								(*outVertices)[vertexOffset + v].weights = XMFLOAT4(weights[0], weights[1], weights[2], weights[3]);
	//							}
	//						}
	//					}
	//				}
	//				// --- Чтение индексов ---
	//				if (prim->indices) {
	//					cgltf_accessor* acc = prim->indices;
	//					char* buffer_base = (char*)acc->buffer_view->buffer->data;
	//					size_t total_offset = acc->buffer_view->offset + acc->offset;
	//					void* index_ptr = (void*)(buffer_base + total_offset);

	//					size_t stride = acc->buffer_view->stride;

	//					for (size_t idx = 0; idx < acc->count; ++idx) {
	//						uint32_t raw_index = 0;

	//						// Определение типа индекса (16-бит или 32-бит)
	//						if (acc->component_type == cgltf_component_type_r_16u) {
	//							size_t current_stride = stride ? stride : sizeof(uint16_t);
	//							raw_index = *(uint16_t*)((char*)index_ptr + idx * current_stride);
	//						}
	//						else if (acc->component_type == cgltf_component_type_r_32u) {
	//							size_t current_stride = stride ? stride : sizeof(uint32_t);
	//							raw_index = *(uint32_t*)((char*)index_ptr + idx * current_stride);
	//						}
	//						else if (acc->component_type == cgltf_component_type_r_8u) {
	//							size_t current_stride = stride ? stride : sizeof(uint8_t);
	//							raw_index = *(uint8_t*)((char*)index_ptr + idx * current_stride);
	//						}

	//						// Смещение индекса с учетом уже добавленных вершин из прошлых примитивов
	//						uint32_t final_index = raw_index + (uint32_t)vertexOffset;

	//						// Запись в вашу структуру float4 (как указано в ТЗ)
	//						if (idx % 3 == 0) (*outIndices)[indexOffset/3 + idx/3].index.x = (float)final_index;
	//						if (idx % 3 == 1) (*outIndices)[indexOffset/3 + idx/3].index.y = (float)final_index;
	//						if (idx % 3 == 2) (*outIndices)[indexOffset/3 + idx/3].index.z = (float)final_index;
	//						(*outIndices)[indexOffset / 3 + idx / 3].index.w = 0;
	//					}
	//					indexOffset += acc->count;
	//				}
	//				else if (prim_vertex_count >= 3) {
	//					for (size_t tri = 0; tri < prim_vertex_count / 3; ++tri) {
	//						(*outIndices)[indexOffset / 3 + tri].index = float4{
	//							(float)(vertexOffset + tri * 3 + 0),
	//							(float)(vertexOffset + tri * 3 + 1),
	//							(float)(vertexOffset + tri * 3 + 2),
	//							0.0f
	//						};
	//					}
	//					indexOffset += (prim_vertex_count / 3) * 3;
	//				}

	//				// Сдвигаем глобальный офсет вершин для следующего примитива
	//				vertexOffset += prim_vertex_count;

	//			}
	//		}
	//	


	//		cgltf_free(data);
	//	}
	//	return result == cgltf_result_success && vCount > 0 && iCount > 0;
	//}


	/*
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
		if (LoadObjToPointersGLTF(name, &vArray, &iArray, vertexCount, triangleCount))
		{
			gltfAnim::scene.modelPath = name ? name : "";
			gltfAnim::scene.animationPath.clear();
			gltfAnim::scene.status = "Model loaded";
			if (vertexCount > 0)
			{
				float xMax = vArray[0].position.x;
				float xMin = vArray[0].position.x;
				float yMax = vArray[0].position.y;
				float yMin = vArray[0].position.y;
				float zMax = vArray[0].position.z;
				float zMin = vArray[0].position.z;

				for (int i = 1; i < vertexCount; i++)
				{
					xMax = max(xMax, vArray[i].position.x);
					xMin = min(xMin, vArray[i].position.x);
					yMax = max(yMax, vArray[i].position.y);
					yMin = min(yMin, vArray[i].position.y);
					zMax = max(zMax, vArray[i].position.z);
					zMin = min(zMin, vArray[i].position.z);
				}

				float xCenter = (xMax + xMin) / 2.0f;
				float yCenter = (yMax + yMin) / 2.0f;
				float zCenter = (zMax + zMin) / 2.0f;
				float xSize = xMax - xMin;
				float ySize = yMax - yMin;
				float zSize = zMax - zMin;
				float maxSize = max(max(xSize, ySize), zSize);
				float scale = maxSize > 0.00001f ? (4.0f / maxSize) : 1.0f;

				gltfAnim::scene.modelCenterScale = XMFLOAT4(xCenter, yCenter, zCenter, scale);
			}

			CreateSB(0, sizeof(vertex), vertexCount, vArray);
			CreateSB(1, sizeof(index), triangleCount, iArray);
		}
			else
		{
			gltfAnim::scene.status = "Model load failed";
		}
	}

	bool LoadAnimations(const char* name, bool replaceExisting = true)
	{
		cgltf_options options = { 0 };
		cgltf_data* data = NULL;
		cgltf_result result = cgltf_parse_file(&options, name, &data);
		if (result != cgltf_result_success)
		{
			return false;
		}

		result = cgltf_load_buffers(&options, data, name);
		if (result != cgltf_result_success)
		{
			cgltf_free(data);
			return false;
		}
		bool loaded = gltfAnim::ReadAnimations(data, replaceExisting, true);
		gltfAnim::scene.animationPath = name ? name : "";
		gltfAnim::scene.status = loaded ? "Animation loaded" : "Animation skeleton incompatible";
		cgltf_free(data);
		return loaded;
	}

	bool LoadAnimations(const std::vector<std::string>& names)
	{
		bool loadedAny = false;
		for (size_t i = 0; i < names.size(); ++i)
		{
			loadedAny = LoadAnimations(names[i].c_str(), i == 0) || loadedAny;
		}

		if (loadedAny)
		{
			if (names.size() == 1)
			{
				gltfAnim::scene.animationPath = names[0];
			}
			else
			{
				gltfAnim::scene.animationPath = std::to_string(names.size()) + " animation files";
			}
			gltfAnim::scene.status = "Animations loaded";
		}
		return loadedAny;
	}
	*/

#endif

	void Init()
	{
		paramConstBufInit();

		Create(buffer[(int)cBuffer::camera], sizeof(camera));
		Create(buffer[(int)cBuffer::frame], sizeof(frame));
		Create(buffer[(int)cBuffer::global], sizeof(global));
		Create(buffer[(int)cBuffer::extra], sizeof(extra));
		gltfAnim::CreateBoneBuffer(device);

#if EditMode

		//LoadObjToPointers("projectFiles//hero.obj", &vArray, &iArray, vertexCount, triangleCount);

		//CreateSB(0, sizeof(vertex), vertexCount, vArray);
		//CreateSB(1, sizeof(index), triangleCount, iArray);
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

