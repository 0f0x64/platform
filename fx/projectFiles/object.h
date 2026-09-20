float distance(const float4& p1, const float4& p2) {
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	float dz = p2.z - p1.z;

	return sqrt(dx * dx + dy * dy + dz * dz);
}

float4 lerp3(const float4& a, const float4& b, float t) {
	return float4{
		a.x + t * (b.x - a.x),
		a.y + t * (b.y - a.y),
		a.z + t * (b.z - a.z),
		a.w // Сохраняем оригинальное значение w из первой точки
	};
}

float frac(float x) {
	return x - floor(x);
}

float4 normalize(const float4& v) {
	// Считаем длину вектора по формуле Пифагора
	float length = ::std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

	// Защита от деления на ноль (если вектор нулевой)
	if (length < 0.00001f) {
		return float4{ 0.0f, 0.0f, 0.0f, v.w };
	}

	// Возвращаем нормализованный вектор
	return float4{
		v.x / length,
		v.y / length,
		v.z / length,
		v.w // Поле w оставляем оригинальным
	};
}

float4 cross(const float4& a, const float4& b) {
	return float4{
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x,
		0.0f // Для векторов направления w обычно равен 0
	};
}

namespace Object {

#define BoneLimit 256
#define DefaultBlendTime 0.35f

	struct mesh {
		bool loaded = false;

		::std::vector<ConstBuf::gltfAnim::Joint> joints;
		::std::vector<ConstBuf::gltfAnim::AnimationClip> animations;
		::std::vector<XMFLOAT4X4> bindLocal;
		//::std::string modelPath;
		//::std::string animationPath;
		float4 modelCenterScale = float4(0, 0, 0, 0);
		XMMATRIX bonePalette[BoneLimit];

		// LookAt state. Values are supplied by inputController.
		float lookYawTarget = 0.0f;
		float lookYawCurrent = 0.0f;
		float lookPitchTarget = 0.0f;
		float lookPitchCurrent = 0.0f;
		bool lookAtEnabled = true;

		XMMATRIX model = XMMatrixIdentity();
		float4 colorTint = { 1.0f, 1.0f, 1.0f, 1.0f };

		ConstBuf::vertex* vArray = nullptr;
		ConstBuf::index* iArray = nullptr;

		uint32_t vertexCount = 0;
		uint32_t triangleCount = 0;

		ID3D11Buffer* pSBuffer[2] = { nullptr, nullptr };
		ID3D11ShaderResourceView* pSB_SRV[2] = { nullptr, nullptr };

		ID3D11Buffer* boneBuffer = nullptr;

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

		inline int NodeIndex(ConstBuf::cgltf_data* data, const ConstBuf::cgltf_node* node)
		{
			if (!node)
			{
				return -1;
			}
			return static_cast<int>(node - data->nodes);
		}

		inline int FindJointByName(const char* name)
		{
			if (!name || !name[0])
			{
				return -1;
			}

			for (size_t i = 0; i < joints.size(); ++i)
			{
				if (joints[i].name == name)
				{
					return static_cast<int>(i);
				}
			}

			return -1;
		}

		inline ::std::string BaseName(const char* path)
		{
			if (!path || !path[0])
			{
				return "";
			}

			::std::string value(path);
			const size_t slash = value.find_last_of("\\/");
			if (slash != ::std::string::npos)
			{
				value = value.substr(slash + 1);
			}
			return value;
		}

		inline ::std::string CanonicalizeJointName(const ::std::string& name)
		{
			if (name.size() > 4)
			{
				const size_t dot = name.size() - 4;
				if (name[dot] == '.' &&
					name[dot + 1] >= '0' && name[dot + 1] <= '9' &&
					name[dot + 2] >= '0' && name[dot + 2] <= '9' &&
					name[dot + 3] >= '0' && name[dot + 3] <= '9')
				{
					return name.substr(0, dot);
				}
			}
			return name;
		}

		inline int ResolveAnimationTargetJoint(ConstBuf::cgltf_data* data, ConstBuf::cgltf_node* node, bool remapToCurrentSkeleton)
		{
			if (!remapToCurrentSkeleton)
			{
				return NodeIndex(data, node);
			}

			const int byName = FindJointByName(node ? node->name : nullptr);
			if (byName >= 0)
			{
				return byName;
			}

			if (node && node->name)
			{
				const ::std::string canonical = CanonicalizeJointName(node->name);
				for (size_t i = 0; i < joints.size(); ++i)
				{
					if (CanonicalizeJointName(joints[i].name) == canonical)
					{
						return static_cast<int>(i);
					}
				}
			}

			const int byIndex = NodeIndex(data, node);
			if (byIndex >= 0 && byIndex < static_cast<int>(joints.size()))
			{
				return byIndex;
			}

			return -1;
		}

		inline void ResolveGlobalPoseJoint(size_t idx, ::std::vector<char>& resolved, ::std::vector<char>& inStack)
		{
			if (idx >= joints.size() || resolved[idx])
			{
				return;
			}

			if (inStack[idx])
			{
				joints[idx].global = joints[idx].local;
				resolved[idx] = 1;
				return;
			}

			inStack[idx] = 1;

			const int parentIdx = joints[idx].parent;
			if (parentIdx < 0 || static_cast<size_t>(parentIdx) >= joints.size())
			{
				joints[idx].global = joints[idx].local;
			}
			else
			{
				ResolveGlobalPoseJoint(static_cast<size_t>(parentIdx), resolved, inStack);

				const XMMATRIX parent = XMLoadFloat4x4(&joints[parentIdx].global);
				const XMMATRIX local = XMLoadFloat4x4(&joints[idx].local);
				XMStoreFloat4x4(&joints[idx].global, local * parent);
			}

			inStack[idx] = 0;
			resolved[idx] = 1;
		}

		inline XMMATRIX ReadNodeLocal(const ConstBuf::cgltf_node& node)
		{
			if (node.has_matrix)
			{
				XMFLOAT4X4 m{};
				for (int r = 0; r < 4; ++r)
				{
					for (int c = 0; c < 4; ++c)
					{
						m.m[r][c] = node.matrix[r * 4 + c];
					}
				}
				return XMLoadFloat4x4(&m);
			}

			XMVECTOR translation = XMVectorZero();
			if (node.has_translation)
			{
				translation = XMVectorSet(node.translation[0], node.translation[1], node.translation[2], 0.0f);
			}

			XMVECTOR rotation = XMQuaternionIdentity();
			if (node.has_rotation)
			{
				rotation = XMQuaternionNormalize(
					XMVectorSet(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]));
			}

			XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
			if (node.has_scale)
			{
				scale = XMVectorSet(node.scale[0], node.scale[1], node.scale[2], 1.0f);
			}

			return XMMatrixScalingFromVector(scale) *
				XMMatrixRotationQuaternion(rotation) *
				XMMatrixTranslationFromVector(translation);
		}

		inline void FillSkinDefaults(ConstBuf::vertex& out)
		{
			out.joints = XMUINT4(0, 0, 0, 0);
			out.weights = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
		}

		inline void UpdateGlobalPose()
		{
			if (joints.empty())
			{
				return;
			}

			::std::vector<char> resolved(joints.size(), 0);
			::std::vector<char> inStack(joints.size(), 0);

			for (size_t i = 0; i < joints.size(); ++i)
			{
				ResolveGlobalPoseJoint(i, resolved, inStack);
			}
		}

		inline void BuildBonePalette()
		{
			for (int i = 0; i < BoneLimit; ++i)
			{
				bonePalette[i] = XMMatrixIdentity();
			}

			const size_t count = ::std::min<size_t>(joints.size(), BoneLimit);
			for (size_t i = 0; i < count; ++i)
			{
				const XMMATRIX global = XMLoadFloat4x4(&joints[i].global);
				const XMMATRIX inverseBind = XMLoadFloat4x4(&joints[i].inverseBind);
				bonePalette[i] = XMMatrixTranspose(inverseBind * global);
			}
		}

		inline void ResetToBindPose()
		{
			for (size_t i = 0; i < joints.size() && i < bindLocal.size(); ++i)
			{
				joints[i].local = bindLocal[i];
			}
			UpdateGlobalPose();
		}

		inline bool ReadAnimations(ConstBuf::cgltf_data* data, bool replaceExisting = true, bool remapToCurrentSkeleton = false)
		{
			if (replaceExisting)
			{
				animations.clear();
			}

			const size_t oldCount = animations.size();

			for (ConstBuf::cgltf_size ai = 0; ai < data->animations_count; ++ai)
			{
				ConstBuf::cgltf_animation& src = data->animations[ai];
				ConstBuf::gltfAnim::AnimationClip clip;
				clip.name = src.name ? src.name : "";

				for (ConstBuf::cgltf_size ci = 0; ci < src.channels_count; ++ci)
				{
					ConstBuf::cgltf_animation_channel& srcChannel = src.channels[ci];
					if (!srcChannel.sampler || !srcChannel.target_node)
					{
						continue;
					}

					ConstBuf::cgltf_animation_sampler& sampler = *srcChannel.sampler;
					if (!sampler.input || !sampler.output)
					{
						continue;
					}

					ConstBuf::gltfAnim::AnimationChannel channel;
					if (remapToCurrentSkeleton)
					{
						channel.joint = ResolveAnimationTargetJoint(data, srcChannel.target_node, true);
					}
					else
					{
						channel.joint = ResolveAnimationTargetJoint(data, srcChannel.target_node, false);
					}
					if (channel.joint < 0)
					{
						continue;
					}
					channel.path = srcChannel.target_path;
					channel.times.resize(sampler.input->count);

					for (ConstBuf::cgltf_size i = 0; i < sampler.input->count; ++i)
					{
						float value = 0.0f;
						ConstBuf::cgltf_accessor_read_float(sampler.input, i, &value, 1);
						channel.times[i] = value;
						clip.duration = (::std::max)(clip.duration, value);
					}

					const bool isRotation = channel.path == ConstBuf::cgltf_animation_path_type_rotation;
					const bool isCubicSpline = sampler.interpolation == ConstBuf::cgltf_interpolation_type_cubic_spline;
					channel.values.resize(channel.times.size());
					for (size_t i = 0; i < channel.times.size(); ++i)
					{
						ConstBuf::cgltf_size sampleIndex = isCubicSpline ? static_cast<ConstBuf::cgltf_size>(i * 3 + 1) : static_cast<ConstBuf::cgltf_size>(i);
						if (sampleIndex >= sampler.output->count) sampleIndex = sampler.output->count - 1;

						float values[4]{ 0.0f, 0.0f, 0.0f, isRotation ? 1.0f : 0.0f };
						ConstBuf::cgltf_accessor_read_float(sampler.output, sampleIndex, values, isRotation ? 4 : 3);
						channel.values[i] = XMFLOAT4(values[0], values[1], values[2], values[3]);
					}

					clip.channels.push_back(::std::move(channel));
				}

				if (!clip.channels.empty())
				{
					animations.push_back(::std::move(clip));
				}
			}

			const bool added = animations.size() > oldCount;
			if (added)
			{
				int animId = static_cast<int>(oldCount);

				ConstBuf::gltfAnim::AnimationClip& clip = animations[animId];
				clip.currentTime = 0.0f;
				ResetToBindPose();
				BuildBonePalette();
			}
			return added;
		}

		inline bool LoadAnimationFile(const char* path, bool remapToCurrentSkeleton = true)
		{
			ConstBuf::cgltf_options opts{};
			ConstBuf::cgltf_data* data = nullptr;

			if (ConstBuf::cgltf_parse_file(&opts, path, &data) != ConstBuf::cgltf_result_success)
				return false;

			if (ConstBuf::cgltf_load_buffers(&opts, data, path) != ConstBuf::cgltf_result_success)
			{
				ConstBuf::cgltf_free(data);
				return false;
			}

			bool added = ReadAnimations(data, false, remapToCurrentSkeleton);
			ConstBuf::cgltf_free(data);
			return added;
		}

		inline void CreateBoneBuffer(ID3D11Device* device)
		{
			if (boneBuffer)
			{
				return;
			}

			D3D11_BUFFER_DESC desc{};
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(bonePalette);
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.StructureByteStride = 16;
			device->CreateBuffer(&desc, nullptr, &boneBuffer);
		}

		inline void BindBones(ID3D11DeviceContext* context)
		{
			if (!boneBuffer)
			{
				return;
			}

			context->UpdateSubresource(boneBuffer, 0, nullptr, bonePalette, 0, 0);
			context->VSSetConstantBuffers(4, 1, &boneBuffer);
		}

		inline void ReadSkeleton(ConstBuf::cgltf_data* data)
		{
			joints.clear();
			bindLocal.clear();

			joints.resize(data->nodes_count);
			bindLocal.resize(data->nodes_count);

			for (ConstBuf::cgltf_size i = 0; i < data->nodes_count; ++i)
			{
				ConstBuf::cgltf_node& node = data->nodes[i];
				ConstBuf::gltfAnim::Joint& joint = joints[i];
				joint.name = node.name ? node.name : "";
				joint.parent = -1;
				XMMATRIX local = ReadNodeLocal(node);
				XMStoreFloat4x4(&joint.local, local);
				XMStoreFloat4x4(&joint.global, local);
				XMStoreFloat4x4(&joint.inverseBind, XMMatrixIdentity());
				bindLocal[i] = joint.local;
			}

			for (ConstBuf::cgltf_size i = 0; i < data->nodes_count; ++i)
			{
				ConstBuf::cgltf_node& node = data->nodes[i];
				for (ConstBuf::cgltf_size c = 0; c < node.children_count; ++c)
				{
					const int child = NodeIndex(data, node.children[c]);
					if (child >= 0 && child < static_cast<int>(joints.size()))
					{
						joints[child].parent = static_cast<int>(i);
					}
				}
			}

			UpdateGlobalPose();

			for (size_t i = 0; i < joints.size(); ++i)
			{
				const XMMATRIX global = XMLoadFloat4x4(&joints[i].global);
				XMStoreFloat4x4(&joints[i].inverseBind, XMMatrixInverse(nullptr, global));
			}

			if (data->skins_count > 0)
			{
				ConstBuf::cgltf_skin& skin = data->skins[0];
				if (skin.inverse_bind_matrices)
				{
					for (ConstBuf::cgltf_size i = 0; i < skin.joints_count; ++i)
					{
						const int nodeIndex = NodeIndex(data, skin.joints[i]);
						if (nodeIndex < 0 || nodeIndex >= static_cast<int>(joints.size()))
						{
							continue;
						}

						float values[16]{};
						if (cgltf_accessor_read_float(skin.inverse_bind_matrices, i, values, 16))
						{
							XMFLOAT4X4 ib{};
							for (int r = 0; r < 4; ++r)
							{
								for (int c = 0; c < 4; ++c)
								{
									ib.m[r][c] = values[r * 4 + c];
								}
							}
							joints[nodeIndex].inverseBind = ib;
						}
					}
				}
			}

			BuildBonePalette();
		}

		bool LoadObjToPointersGLTF(const ::std::string& filename, ConstBuf::vertex** outVertices, ConstBuf::index** outIndices)
		{
			ConstBuf::cgltf_options options = { 0 };
			ConstBuf::cgltf_data* data = NULL;
			ConstBuf::cgltf_result result = cgltf_parse_file(&options, filename.c_str(), &data);

			if (result == ConstBuf::cgltf_result_success)
			{

				result = cgltf_load_buffers(&options, data, filename.c_str());
				if (result != ConstBuf::cgltf_result_success) {
					return false;
				}

				ReadSkeleton(data);
				ReadAnimations(data);

				vertexCount = 0;
				triangleCount = 0;

				for (size_t i = 0; i < data->meshes_count; ++i) {
					ConstBuf::cgltf_mesh* mesh = &data->meshes[i];

					for (size_t j = 0; j < mesh->primitives_count; ++j) {
						ConstBuf::cgltf_primitive* prim = &mesh->primitives[j];

						// Ищем атрибут позиции для подсчета вершин
						for (size_t k = 0; k < prim->attributes_count; ++k) {
							if (prim->attributes[k].type == ConstBuf::cgltf_attribute_type_position) {
								vertexCount += prim->attributes[k].data->count;
								break;
							}
						}
						// Считаем индексы
						if (prim->indices) {
							triangleCount += (uint32_t)(prim->indices->count / 3);
						}
						else {
							for (size_t k = 0; k < prim->attributes_count; ++k) {
								if (prim->attributes[k].type == ConstBuf::cgltf_attribute_type_position) {
									triangleCount += (uint32_t)(prim->attributes[k].data->count / 3);
									break;
								}
							}
						}
					}
				}


				// 2. Выделение памяти по вашему шаблону
				if (*outVertices) {
					delete[] * outVertices;
					*outVertices = nullptr;
				}
				if (*outIndices) {
					delete[] * outIndices;
					*outIndices = nullptr;
				}

				if (vertexCount == 0) return false;

				*outVertices = new ConstBuf::vertex[vertexCount];
				if (triangleCount > 0) {
					*outIndices = new ConstBuf::index[triangleCount];
				}

				// 3. Второй проход: заполнение массивов данные

				size_t vertexOffset = 0;
				size_t indexOffset = 0;

				for (size_t i = 0; i < data->meshes_count; ++i)
				{
					ConstBuf::cgltf_mesh* mesh = &data->meshes[i];

					for (size_t j = 0; j < mesh->primitives_count; ++j) {
						ConstBuf::cgltf_primitive* prim = &mesh->primitives[j];
						size_t prim_vertex_count = 0;



						// --- Чтение вершин (Позиции) ---
						for (size_t k = 0; k < prim->attributes_count; ++k) {
							if (prim->attributes[k].type == ConstBuf::cgltf_attribute_type_position) {
								ConstBuf::cgltf_accessor* acc = prim->attributes[k].data;
								prim_vertex_count = acc->count;

								for (size_t v = 0; v < prim_vertex_count; ++v) {
									FillSkinDefaults((*outVertices)[vertexOffset + v]);
									float position_element[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

									// Безопасная функция cgltf сама учитывает stride, offset, sparse данные и тип компонента
									if (cgltf_accessor_read_float(acc, v, position_element, 4)) {
										(*outVertices)[vertexOffset + v].position = float4{
											position_element[0],
											position_element[1],
											position_element[2],
											(float)i
										};
									}
									else {
										// Если не удалось прочитать, пишем нули во избежание мусора
										(*outVertices)[vertexOffset + v].position = float4{ 0.0f, 0.0f, 0.0f, 1.0f };
									}
								}
								break;
							}
						}

						// --- Skinning data ---
						for (size_t k = 0; k < prim->attributes_count; ++k) {
							ConstBuf::cgltf_attribute* attr = &prim->attributes[k];
							if (attr->type == ConstBuf::cgltf_attribute_type_joints) {
								ConstBuf::cgltf_accessor* acc = attr->data;
								for (size_t v = 0; v < acc->count && v < prim_vertex_count; ++v) {
									ConstBuf::cgltf_uint joints[4] = { 0, 0, 0, 0 };
									if (cgltf_accessor_read_uint(acc, v, joints, 4)) {
										XMUINT4 mapped(0, 0, 0, 0);
										for (int c = 0; c < 4; ++c) {
											uint32_t jointIndex = joints[c];
											if (data->skins_count > 0 && jointIndex < data->skins[0].joints_count) {
												jointIndex = NodeIndex(data, data->skins[0].joints[jointIndex]);
											}
											if (c == 0) mapped.x = jointIndex; else if (c == 1) mapped.y = jointIndex; else if (c == 2) mapped.z = jointIndex; else mapped.w = jointIndex;
										}
										(*outVertices)[vertexOffset + v].joints = mapped;
									}
								}
							}
							else if (attr->type == ConstBuf::cgltf_attribute_type_weights) {
								ConstBuf::cgltf_accessor* acc = attr->data;
								for (size_t v = 0; v < acc->count && v < prim_vertex_count; ++v) {
									float weights[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
									if (ConstBuf::cgltf_accessor_read_float(acc, v, weights, 4)) {
										float sum = weights[0] + weights[1] + weights[2] + weights[3];
										if (sum > 0.000001f) {
											weights[0] /= sum;
											weights[1] /= sum;
											weights[2] /= sum;
											weights[3] /= sum;
										}
										(*outVertices)[vertexOffset + v].weights = XMFLOAT4(weights[0], weights[1], weights[2], weights[3]);
									}
								}
							}
						}
						// --- Чтение индексов ---
						if (prim->indices) {
							ConstBuf::cgltf_accessor* acc = prim->indices;
							char* buffer_base = (char*)acc->buffer_view->buffer->data;
							size_t total_offset = acc->buffer_view->offset + acc->offset;
							void* index_ptr = (void*)(buffer_base + total_offset);

							size_t stride = acc->buffer_view->stride;

							for (size_t idx = 0; idx < acc->count; ++idx) {
								uint32_t raw_index = 0;

								// Определение типа индекса (16-бит или 32-бит)
								if (acc->component_type == ConstBuf::cgltf_component_type_r_16u) {
									size_t current_stride = stride ? stride : sizeof(uint16_t);
									raw_index = *(uint16_t*)((char*)index_ptr + idx * current_stride);
								}
								else if (acc->component_type == ConstBuf::cgltf_component_type_r_32u) {
									size_t current_stride = stride ? stride : sizeof(uint32_t);
									raw_index = *(uint32_t*)((char*)index_ptr + idx * current_stride);
								}
								else if (acc->component_type == ConstBuf::cgltf_component_type_r_8u) {
									size_t current_stride = stride ? stride : sizeof(uint8_t);
									raw_index = *(uint8_t*)((char*)index_ptr + idx * current_stride);
								}

								// Смещение индекса с учетом уже добавленных вершин из прошлых примитивов
								uint32_t final_index = raw_index + (uint32_t)vertexOffset;

								// Запись в вашу структуру float4 (как указано в ТЗ)
								if (idx % 3 == 0) (*outIndices)[indexOffset / 3 + idx / 3].index.x = (float)final_index;
								if (idx % 3 == 1) (*outIndices)[indexOffset / 3 + idx / 3].index.y = (float)final_index;
								if (idx % 3 == 2) (*outIndices)[indexOffset / 3 + idx / 3].index.z = (float)final_index;
								(*outIndices)[indexOffset / 3 + idx / 3].index.w = 0;
							}
							indexOffset += acc->count;
						}
						else if (prim_vertex_count >= 3) {
							for (size_t tri = 0; tri < prim_vertex_count / 3; ++tri) {
								(*outIndices)[indexOffset / 3 + tri].index = float4{
									(float)(vertexOffset + tri * 3 + 0),
									(float)(vertexOffset + tri * 3 + 1),
									(float)(vertexOffset + tri * 3 + 2),
									0.0f
								};
							}
							indexOffset += (prim_vertex_count / 3) * 3;
						}

						// Сдвигаем глобальный офсет вершин для следующего примитива
						vertexOffset += prim_vertex_count;

					}
				}



				ConstBuf::cgltf_free(data);
			}
			else {
				switch (result) {
				case ConstBuf::cgltf_result_file_not_found:
					Log("cgltf error: file not found: %s\n", filename.c_str());
					break;
				case ConstBuf::cgltf_result_io_error:
					Log("cgltf error: IO error reading: %s\n", filename.c_str());
					break;
				case ConstBuf::cgltf_result_invalid_json:
					Log("cgltf error: invalid JSON in: %s\n", filename.c_str());
					break;
				case ConstBuf::cgltf_result_invalid_gltf:
					Log("cgltf error: invalid glTF in: %s\n", filename.c_str());
					break;
				default:
					Log("cgltf error: unknown error %d for: %s\n", filename.c_str());
					break;
				}
			}

			return result == ConstBuf::cgltf_result_success && vertexCount > 0 && triangleCount > 0;
		}

		void LoadObj(const char* name)
		{
			if (loaded) {
				Log("Error while loading model ");
				Log(name);
				Log(". This buffer is already loaded\n");
				return;
			};

			if (LoadObjToPointersGLTF(name, &vArray, &iArray))
			{
				ConstBuf::gltfAnim::scene.modelPath = name ? name : "";
				ConstBuf::gltfAnim::scene.animationPath.clear();
				ConstBuf::gltfAnim::scene.status = "Model loaded";
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

					modelCenterScale = float4(xCenter, yCenter, zCenter, scale);
				}

				CreateSB(0, sizeof(ConstBuf::vertex), vertexCount, vArray);
				CreateSB(1, sizeof(ConstBuf::index), triangleCount, iArray);

				CreateBoneBuffer(device);

				loaded = true;
				Log("GLTF model ");
				Log(name);
				Log(" loaded successfully\n");
			}
			else
			{
				ConstBuf::gltfAnim::scene.status = "Model load failed";
				Log("GLTF model ");
				Log(name);
				Log(" load failed\n");
			}
		}

		void LoadToShaders() {
			BindSB(0);
			BindSB(1);
		}

		inline void SetLookAtYaw(float yaw)
		{
			ConstBuf::gltfAnim::lookAtConfig.Resolve(joints);

			lookYawTarget =
				std::clamp(
					yaw,
					-ConstBuf::gltfAnim::lookAtConfig.maxYaw,
					ConstBuf::gltfAnim::lookAtConfig.maxYaw);
		}

		inline void SetLookAtPitch(float pitch)
		{
			ConstBuf::gltfAnim::lookAtConfig.Resolve(joints);

			lookPitchTarget =
				std::clamp(
					pitch,
					-ConstBuf::gltfAnim::lookAtConfig.maxPitch,
					ConstBuf::gltfAnim::lookAtConfig.maxPitch);
		}

		inline void SetLookAtEnabled(bool enabled)
		{
			lookAtEnabled = enabled;
		}

		inline void ResetLookAtPose()
		{
			lookYawTarget = 0.0f;
			lookYawCurrent = 0.0f;

			lookPitchTarget = 0.0f;
			lookPitchCurrent = 0.0f;
		}

		inline void ApplyLookAtRotation(float deltaTime)
		{
			if (!lookAtEnabled || joints.empty())
				return;

			ConstBuf::gltfAnim::lookAtConfig.Resolve(joints);
			if (ConstBuf::gltfAnim::lookAtConfig.headIdx < 0 ||
				ConstBuf::gltfAnim::lookAtConfig.headIdx >= static_cast<int>(joints.size()))
			{
				return;
			}

			lookYawTarget = std::clamp(
				lookYawTarget,
				-ConstBuf::gltfAnim::lookAtConfig.maxYaw,
				ConstBuf::gltfAnim::lookAtConfig.maxYaw);
			lookPitchTarget = std::clamp(
				lookPitchTarget,
				-ConstBuf::gltfAnim::lookAtConfig.maxPitch,
				ConstBuf::gltfAnim::lookAtConfig.maxPitch);

			lookYawCurrent = lookYawTarget;
			lookPitchCurrent = lookPitchTarget;

			::std::vector<int> chain;
			int jointIdx = ConstBuf::gltfAnim::lookAtConfig.headIdx;
			while (jointIdx >= 0 && jointIdx < static_cast<int>(joints.size()))
			{
				chain.push_back(jointIdx);
				if (jointIdx == ConstBuf::gltfAnim::lookAtConfig.hipsIdx)
					break;
				jointIdx = joints[jointIdx].parent;
			}

			if (chain.empty())
				return;

			const float lastIndex = static_cast<float>(chain.size() - 1);
			const float bodyFollowPower = 0.55f;

			for (int chainIndex = static_cast<int>(chain.size()) - 1;
				chainIndex >= 0;
				--chainIndex)
			{
				jointIdx = chain[chainIndex];

				const float linearPosition = lastIndex > 0.0f
					? 1.0f - static_cast<float>(chainIndex) / lastIndex
					: 1.0f;
				const float position = powf(linearPosition, bodyFollowPower);
				const float parentLinearPosition = chainIndex + 1 < static_cast<int>(chain.size())
					? 1.0f - static_cast<float>(chainIndex + 1) / lastIndex
					: 0.0f;
				const float parentPosition = powf(parentLinearPosition, bodyFollowPower);
				const float segmentWeight = position - parentPosition;

				XMMATRIX local = XMLoadFloat4x4(&joints[jointIdx].local);
				XMVECTOR scale;
				XMVECTOR rotation;
				XMVECTOR translation;
				if (!XMMatrixDecompose(&scale, &rotation, &translation, local))
					continue;

				const float yaw = -lookYawTarget * segmentWeight;
				const float pitch = lookPitchTarget * segmentWeight;
				XMVECTOR qYaw = XMQuaternionRotationAxis(
					XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), yaw);
				XMVECTOR qPitch = XMQuaternionRotationAxis(
					XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), pitch);
				XMVECTOR delta = XMQuaternionNormalize(XMQuaternionMultiply(qYaw, qPitch));
				XMVECTOR newRotation = XMQuaternionNormalize(
					XMQuaternionMultiply(rotation, delta));

				XMMATRIX newLocal =
					XMMatrixScalingFromVector(scale) *
					XMMatrixRotationQuaternion(newRotation) *
					XMMatrixTranslationFromVector(translation);
				XMStoreFloat4x4(&joints[jointIdx].local, newLocal);
			}
		}

		inline bool IsLookAtUpperBodyJoint(int jointIndex)
		{
			ConstBuf::gltfAnim::lookAtConfig.Resolve(joints);

			int current = ConstBuf::gltfAnim::lookAtConfig.headIdx;
			while (current >= 0 && current < static_cast<int>(joints.size()))
			{
				if (current == jointIndex)
					return true;
				current = joints[current].parent;
			}

			return false;
		}

		inline void PlayAnimation(int id, float time = DefaultBlendTime) {
			ConstBuf::gltfAnim::AnimationClip& clip = animations[id];

			if (clip.isPlaying)
				return;

			//clip.realWeight = 0.0f;
			clip.currentTime = 0.0f;
			clip.isPlaying = true;

			ConstBuf::interp::Animate(clip.realWeight, 1.0f, time);
		}

		inline void StopAnimation(int id, float time = DefaultBlendTime) {
			ConstBuf::gltfAnim::AnimationClip& clip = animations[id];

			if (!clip.isPlaying)
				return;

			clip.isPlaying = false;

			ConstBuf::interp::Animate(clip.realWeight, 0.0f, time);
		}

		inline void Update(float deltaTime)
		{
			ResetToBindPose();

			if (animations.empty() || joints.empty())
			{
				BuildBonePalette();
				return;
			}

			// Проверяем, есть ли хоть один играющий клип
			bool anyPlaying = false;
			for (ConstBuf::gltfAnim::AnimationClip& clip : animations)
			{
				if ((clip.isPlaying || clip.realWeight > 0.0f) && clip.weight > 0.0f && clip.duration > 0.0f)
				{
					anyPlaying = true;
				}
			}

			if (!anyPlaying)
			{
				BuildBonePalette();
				return;
			}

			// Накопители для смешивания
			::std::vector<XMVECTOR> accumScale(joints.size(), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
			::std::vector<XMVECTOR> accumRotation(joints.size(), XMQuaternionIdentity());
			::std::vector<XMVECTOR> accumTranslation(joints.size(), XMVectorZero());
			::std::vector<bool> jointAnimated(joints.size(), false);

			::std::vector<float> jointWeightSum(joints.size(), 0.0f);

			struct OverridePose
			{
				float priority = 0.0f;
				float blend = 0.0f;
				::std::vector<XMVECTOR> scale;
				::std::vector<XMVECTOR> rotation;
				::std::vector<XMVECTOR> translation;
				::std::vector<bool> animated;
			};

			::std::vector<OverridePose> overridePoses;

			// Проходим по всем клипам
			for (size_t i = 0; i < animations.size(); i++)
			{
				ConstBuf::gltfAnim::AnimationClip& clip = animations[i];

				if ((!clip.isPlaying && clip.realWeight <= 0.0f) || clip.weight <= 0.0f || clip.duration <= 0.0f)
				{
					continue;
				}

				// Обновляем время клипа
				if (clip.looped)
				{
					clip.currentTime = fmodf(clip.currentTime + deltaTime * clip.speed, clip.duration);
				}
				else
				{
					clip.currentTime += deltaTime * clip.speed;
					if (clip.currentTime > clip.duration || clip.currentTime < 0.0f)
					{
						clip.currentTime = clamp(clip.currentTime, 0.0f, clip.duration);
						StopAnimation(i);
						//continue;
					}
				}

				const float realWeight = clamp(clip.realWeight, 0.0f, 1.0f);
				const bool isOverride = clip.weight > 1.0f;
				const float effectiveWeight = clip.weight * realWeight;
				if (realWeight <= 0.0f)
				{
					continue;
				}

				OverridePose* overridePose = nullptr;
				if (isOverride)
				{
					overridePoses.emplace_back();
					overridePose = &overridePoses.back();
					overridePose->priority = clip.weight;
					overridePose->blend = realWeight;
					overridePose->scale.assign(joints.size(), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
					overridePose->rotation.assign(joints.size(), XMQuaternionIdentity());
					overridePose->translation.assign(joints.size(), XMVectorZero());
					overridePose->animated.assign(joints.size(), false);
				}

				// Для каждого joint в этом клипе
				for (size_t jointIdx = 0; jointIdx < joints.size(); ++jointIdx)
				{
					if (i == 7 && IsLookAtUpperBodyJoint(static_cast<int>(jointIdx)))
					{
						continue;
					}

					// Декомпозируем текущую локальную матрицу
					XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
					XMVECTOR rotation = XMQuaternionIdentity();
					XMVECTOR translation = XMVectorZero();
					XMVECTOR decompScale, decompRotation, decompTranslation;
					const XMMATRIX currentLocal = XMLoadFloat4x4(&joints[jointIdx].local);
					if (XMMatrixDecompose(&decompScale, &decompRotation, &decompTranslation, currentLocal))
					{
						scale = decompScale;
						rotation = decompRotation;
						translation = decompTranslation;
					}

					bool animated = false;

					// Ищем каналы для этого joint в данном клипе
					for (const ConstBuf::gltfAnim::AnimationChannel& channel : clip.channels)
					{
						if (channel.joint != static_cast<int>(jointIdx) || channel.times.empty() || channel.values.empty())
						{
							continue;
						}

						animated = true;

						// Поиск ключа
						size_t key = 0;
						for (size_t i = 0; i + 1 < channel.times.size(); ++i)
						{
							key = i;
							if (clip.currentTime < channel.times[i + 1])
							{
								break;
							}
						}

						float alpha = 0.0f;
						XMVECTOR a = XMLoadFloat4(&channel.values[key]);
						XMVECTOR b = a;
						if (key + 1 < channel.times.size())
						{
							const float t0 = channel.times[key];
							const float t1 = channel.times[key + 1];
							if (t1 - t0 > 0.0001f)
							{
								alpha = (::std::max)(0.0f, (::std::min)(1.0f, (clip.currentTime - t0) / (t1 - t0)));
								b = XMLoadFloat4(&channel.values[key + 1]);
							}
						}

						if (channel.path == ConstBuf::cgltf_animation_path_type_translation)
						{
							translation = XMVectorLerp(a, b, alpha);
						}
						else if (channel.path == ConstBuf::cgltf_animation_path_type_rotation)
						{
							a = XMQuaternionNormalize(a);
							b = XMQuaternionNormalize(b);
							if (XMVectorGetX(XMQuaternionDot(a, b)) < 0.0f)
							{
								b = XMVectorNegate(b);
							}
							rotation = XMQuaternionNormalize(XMQuaternionSlerp(a, b, alpha));
						}
						else if (channel.path == ConstBuf::cgltf_animation_path_type_scale)
						{
							//scale = XMVectorLerp(a, b, alpha);
							continue;
						}
					}

					if (animated)
					{
						if (isOverride)
						{
							overridePose->scale[jointIdx] = scale;
							overridePose->rotation[jointIdx] = rotation;
							overridePose->translation[jointIdx] = translation;
							overridePose->animated[jointIdx] = true;
							continue;
						}

						// Взвешенное смешивание
						if (!jointAnimated[jointIdx])
						{
							accumScale[jointIdx] = scale;
							accumRotation[jointIdx] = rotation;
							accumTranslation[jointIdx] = translation;
							jointAnimated[jointIdx] = true;
							jointWeightSum[jointIdx] = effectiveWeight;
						}
						else
						{
							float blend = effectiveWeight / (jointWeightSum[jointIdx] + effectiveWeight);
							accumTranslation[jointIdx] = XMVectorLerp(accumTranslation[jointIdx], translation, blend);
							accumRotation[jointIdx] = XMQuaternionSlerp(accumRotation[jointIdx], rotation, blend);
							// Scale не смешиваем — оставляем от первого клипа
							jointWeightSum[jointIdx] += effectiveWeight;
						}
					}
				}
			}

			// Большие веса в этом проекте задают приоритет override-анимаций.
			// Применяем их после базового бленда: priority задаёт порядок,
			// а realWeight — плавную силу влияния.
			::std::stable_sort(overridePoses.begin(), overridePoses.end(),
				[](const OverridePose& a, const OverridePose& b)
				{
					return a.priority < b.priority;
				});

			for (const OverridePose& pose : overridePoses)
			{
				for (size_t jointIdx = 0; jointIdx < joints.size(); ++jointIdx)
				{
					if (!pose.animated[jointIdx])
					{
						continue;
					}

					if (!jointAnimated[jointIdx])
					{
						XMVECTOR currentScale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
						XMVECTOR currentRotation = XMQuaternionIdentity();
						XMVECTOR currentTranslation = XMVectorZero();
						const bool decomposed = XMMatrixDecompose(
							&currentScale,
							&currentRotation,
							&currentTranslation,
							XMLoadFloat4x4(&joints[jointIdx].local));
						if (!decomposed)
						{
							currentScale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
							currentRotation = XMQuaternionIdentity();
							currentTranslation = XMVectorZero();
						}

						accumScale[jointIdx] = currentScale;
						accumRotation[jointIdx] = currentRotation;
						accumTranslation[jointIdx] = currentTranslation;
						jointAnimated[jointIdx] = true;
					}

					accumScale[jointIdx] = XMVectorLerp(accumScale[jointIdx], pose.scale[jointIdx], pose.blend);
					accumRotation[jointIdx] = XMQuaternionSlerp(accumRotation[jointIdx], pose.rotation[jointIdx], pose.blend);
					accumTranslation[jointIdx] = XMVectorLerp(accumTranslation[jointIdx], pose.translation[jointIdx], pose.blend);
				}
			}

			/*Log(::std::to_string(totalWeight).c_str());
			Log(" | ");
			Log(::std::to_string(invTotalWeight).c_str());
			Log("\n");*/

			// Применяем смешанную позу
			for (size_t jointIdx = 0; jointIdx < joints.size(); ++jointIdx)
			{
				if (!jointAnimated[jointIdx])
				{
					continue;
				}

				// Собираем матрицу
				const XMMATRIX local = XMMatrixScalingFromVector(accumScale[jointIdx]) *
					XMMatrixRotationQuaternion(accumRotation[jointIdx]) *
					XMMatrixTranslationFromVector(accumTranslation[jointIdx]);
				XMStoreFloat4x4(&joints[jointIdx].local, local);
			}

			ApplyLookAtRotation(deltaTime);
			UpdateGlobalPose();
			BuildBonePalette();
		}
	};

	cmd(Show, 
		texture geometry,
		texture normals,
		int8u quality,
		int pos_x,
		int pos_y,
		int pos_z,
		int glow
		)
	{
		reflect;

		#if EditMode //dynamic limits
			auto r = max(Textures::Texture[(int)in.geometry].size.x, Textures::Texture[(int)in.geometry].size.y);
			auto mipMaps = Textures::Texture[(int)in.geometry].mipMaps;
			cmdParamDesc[cmdCounter - 1].param[2]._min = 0;
			cmdParamDesc[cmdCounter - 1].param[2]._max = max((mipMaps ? (UINT)(_log2(r)) : 0)-2,0);
		#endif

		int denom = (int)pow(2, (float)in.quality);
		float q = intToFloatDenom;
		
		int gX = Textures::Texture[(int)in.geometry].size.x / denom;
		int gY = Textures::Texture[(int)in.geometry].size.y / denom;

		vs::objViewer = {

			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(in.pos_x / q, in.pos_y / q, in.pos_z / q)),
				.gX = gX,
				.gY = gY,
				.glow_p = (float)in.glow,
			},

			.textures = {
				.positions = in.geometry,
				.normals = in.normals
				},

			.samplers = {
				.sam1Filter = filter::linear,
				.sam1AddressU = addr::wrap,
				.sam1AddressV = addr::clamp
			}
		};

		vs::objViewer.set();

		ps::basic = 
		{
			.params = {
				#if EditMode
					.hilight = cmdCounter - 1 == hilightedCmd ? 1.f : 0.f
				#else 
					.hilight = 0.f
				#endif
				}

		};

		ps::basicLow =
		{
			.params = {
				#if EditMode
					.hilight = cmdCounter - 1 == hilightedCmd ? 1.f : 0.f
				#else 
					.hilight = 0.f
				#endif
				}

		};

		if (in.glow == 1)
		{
			ps::basic.set();
		}
		else
		{
			ps::basicLow.set();
		}
		

		//Drawer::NullDrawer({(int)gX*(int)gY,1});
		if (in.glow == 0)
		{
			Drawer::NullDrawer({ 1, (int)gX * (int)gY/ 10394 });
		}
		else
		{
			Drawer::NullDrawer({ 1, (int)gX * (int)gY });
		}

		
	}

	

	enum class pMode { point,glow };
	enum class triMode { on,off };
	
	void psModeSet(pMode mode)
	{
		switch (mode)
		{
			case pMode::point:
			{
				ps::basic = { .params = {.hilight = 0.f } };
				ps::basic.set();
				break;
			}
			case pMode::glow:
			{
				ps::basicLow = { .params = {.hilight = 0.f } };
				ps::basicLow.set();
				break;
			}
		}
	}

	void psModeSet2(pMode mode)
	{
		switch (mode)
		{
		case pMode::point:
		{
			ps::basic2 = { .params = {.hilight = 0.f } };
			ps::basic2.set();
			break;
		}
		case pMode::glow:
		{
			ps::basicLow2 = { .params = {.hilight = 0.f } };
			ps::basicLow2.set();
			break;
		}
		}
	}

	void PillarsHand(int count, int skipper, pMode mode)
	{
		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);

		vs::pillarsHand = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
			},
		};

		vs::pillarsHand.set();

		Drawer::NullDrawer({ 1, (int)gX * (int)gY });
	}

	cmd(InsideNebula, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::insideNebula = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color=float4(in.r/100.,in.g/100.,in.b/100.,1)
			},
		};

		vs::insideNebula.set();

		Drawer::NullDrawer({1,(int)gX*(int)gY});

		
	}

	cmd(Blob, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::blob = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::blob.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });

	
	}

	cmd(Pearl, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::pearl = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::pearl.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });

	
	}

	cmd(LeoStar, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::leo = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::leo.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });

	
	}

	cmd(CapStar, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::capriStar = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::capriStar.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });

		
	}

	

	cmd(Tau, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::Tau = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1),
				//.mesh = {
					//#include "girl.h"
					//#include "girl_rand.h"
					//#include "girl_mini.h"
				//}
			},
		};
		
		vs::Tau.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });

		
	}

#if EditMode

	dx11::ConstBuf::sbObject HeroMesh;
	dx11::ConstBuf::sbObject BossMesh;
	dx11::ConstBuf::sbObject* MeshPtr = NULL;

	XMMATRIX heroOnRails;
	//XMMATRIX heroWorld;

	void ShowMesh(mesh* obj, int count, int skipper, pMode mode, int r, int g, int b, triMode tMode, int xPos, int yPos, int zPos, int brightness, int tickness, int zoom, int onLineOfs, int jumpCharge, float deltaTime)
	{

		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);
		float zm = zoom / 100. + 1;

		float4 centerScale = (obj && obj->loaded) ? obj->modelCenterScale : ConstBuf::gltfAnim::scene.modelCenterScale;
		uint32_t triCnt = (obj && obj->loaded) ? obj->triangleCount : ConstBuf::triangleCount;

		vs::girl = {
			.params =
			{
				.model = obj->model,
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
				.base_color = float4(r / 100.,g / 100.,b / 100.,1),
				.colorMultiplier = obj->colorTint,
				.modelPos = float4(xPos / 10000.,yPos / 10000.,zPos / 10000.,0),
				.triCount = float4(triCnt,0,0,0),
				.brightness = float4(brightness,0,0,0),
				.tickness = float4(tickness,0,0,0),
				.modelCenterScale = centerScale,
				.zoom = float4(zm,zm,zm,1),
				.onLineOfs = (float)onLineOfs / 1000.f,
				.jumpCharge = (float)jumpCharge / 100.f,
			},
		};

		if (tMode == triMode::on)
		{
			vs::girl.params.mode = 2;
		}

		vs::girl.set();

		if (obj && obj->loaded) {
			obj->Update(deltaTime); // prev: 1.0f / FRAMES_PER_SECOND
			obj->BindBones(dx11::context);

			obj->LoadToShaders();
		}
		else {
			dx11::ConstBuf::gltfAnim::Update(deltaTime); // prev: 1.0f / FRAMES_PER_SECOND
			dx11::ConstBuf::gltfAnim::BindBones(dx11::context);

			ConstBuf::BindSB(0);
			ConstBuf::BindSB(1);
		}


		if (tMode == triMode::on)
		{
			//dx11::Shaders::resetShader(dx11::Shaders::basic);
			//dx11::Shaders::resetShader(dx11::Shaders::basic);
			Drawer::NullDrawerTri({ count, 1 });
		}
		else
		{
			Drawer::NullDrawer({ 1,(int)gX * (int)gY });
		}


	}

	cmd(Mesh, mesh* obj, int quality, int xPos, int yPos, int zPos, int brightness, int tickness, switcher stencil,int zoom, int onLineOfs, int jumpCharge, float deltaTime)
	{
		reflect;

		int count = 500000;

		DepthBuf::Mode({ depthmode::on });
		BlendMode::Set({
			.mode = blendmode::off,
			.op = blendop::add
			});

		Culling::Set({ cullmode::off });
		if (in.stencil == switcher::on)
		{
			uint32_t triCnt = (in.obj && in.obj->loaded) ? in.obj->triangleCount : ConstBuf::triangleCount;
			ShowMesh(in.obj, (int)triCnt,1,pMode::point,0,0,0, triMode::on, in.xPos, in.yPos, in.zPos,in.brightness,in.tickness,in.zoom,in.onLineOfs, in.jumpCharge, in.deltaTime);
		}

		Culling::Set({ cullmode::off });
		DepthBuf::Mode({ depthmode::readonly });
		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});

		ShowMesh(in.obj, count, 1, pMode::point, 100, 252, 1400, triMode::off, in.xPos, in.yPos, in.zPos, in.brightness, in.tickness,in.zoom, in.onLineOfs, in.jumpCharge, in.deltaTime);
	}

#endif

	cmd(RayHit, float4 pos1, float4 pos2)
	{
		reflect;

		Culling::Set({ cullmode::off });
		DepthBuf::Mode({ depthmode::readonly });
		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});

		vs::line = {
			.params = {
				.gX = 1,
				.gY = 1,
				.pos1 = in.pos1,
				.pos2 = in.pos2
			},
		};

		vs::line.set();
		ps::color.set();

		Drawer::NullDrawer({ 1, 1 });
	}

	cmd(ScorpBall, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::scorpBall = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::scorpBall.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });

		
	}

	cmd(Nebula2, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::Nebula2 = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::Nebula2.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });

		
	}

	cmd(vrg, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		XMMATRIX invViewMatrix = XMMatrixInverse(nullptr, XMMatrixTranspose(ConstBuf::camera.view[0]));
		// Позиция камеры находится в 4-й строке инвертированной матрицы (вектор смещения)
		XMVECTOR cameraPos = invViewMatrix.r[3];
		// Если нужно получить отдельные float:
		XMFLOAT3 eye;
		XMStoreFloat3(&eye, cameraPos);

		XMVECTOR cameraLookAtVec = XMVector3Normalize(invViewMatrix.r[2]);

		// Сохраняем в структуру XMFLOAT3 для передачи в Shader Constants / Constant Buffer
		XMFLOAT3 cameraForward;
		XMStoreFloat3(&cameraForward, cameraLookAtVec);

		// 1. Извлекаем и нормализуем вектор Right (1-я строка инвертированной матрицы)
		XMVECTOR cameraRightVec = XMVector3Normalize(invViewMatrix.r[0]);

		// 2. Извлекаем и нормализуем вектор Up (2-я строка инвертированной матрицы)
		XMVECTOR cameraUpVec = XMVector3Normalize(invViewMatrix.r[1]);

		// Сохраняем в структуры XMFLOAT3 для передачи в ваш Constant Buffer
		XMFLOAT3 cameraRight;
		XMStoreFloat3(&cameraRight, cameraRightVec);

		XMFLOAT3 cameraUp;
		XMStoreFloat3(&cameraUp, cameraUpVec);

		vs::Virgo = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1),
				.eye = float4(eye.x,eye.y,eye.z,0),
				.forward = float4(cameraForward.x,cameraForward.y,cameraForward.z,0),
				.up = float4(cameraUp.x,cameraUp.y,cameraUp.z,0),
				.right = float4(cameraRight.x,cameraRight.y,cameraRight.z,0),
			},
		};

		vs::Virgo.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });

		
	}

	const int smoothPointMAX = 1000;

	struct starline {
		float4 baseColor;
		float4 basePoint[4000];
		float4 point[smoothPointMAX];
		float4 upVector[smoothPointMAX];
		int basePointCount = 0;
		int pointCount = 0;
	};

	struct {
		starline line[2000];
		int lineCount = 0;
	} starLineList;

	int currentLine = -1;
	int currentPoint = 0;
	
	cmd(SetLineCount, int lineCount)
	{
		reflect;
		starLineList.lineCount = in.lineCount;
	}

	const int denom = 1;

	cmd(SetPointPosInLine, int line, int point, int x,int y, int z, int a)
	{
		reflect;
		starLineList.line[in.line].basePoint[in.point] = float4(in.x/ (float)denom,in.y/ (float)denom,in.z/ (float)denom,in.a);
	}

	cmd(AddPointToLine, int x, int y, int z, int a=0)
	{
		reflect;
		starLineList.line[currentLine].basePoint[currentPoint++] = float4(in.x / (float)denom, in.y / (float)denom, in.z / (float)denom, in.a);
		starLineList.line[currentLine].basePointCount = currentPoint;
	}

	void AddPoint( float4 p)
	{
		starLineList.line[currentLine].basePoint[currentPoint++] = p;
		starLineList.line[currentLine].basePointCount = currentPoint;
	}

	



	void smoothStarline(starline& line) {
		line.pointCount = 0; // Сбрасываем старый результат сглаживания

		float totalLength = 0;
		for (int i = 0; i < line.basePointCount-1; i++)
		{
			totalLength += distance(line.basePoint[i], line.basePoint[i + 1]);
		}

		int stepsPerSegment = totalLength/25.;
		if (stepsPerSegment < 2) stepsPerSegment = 2;

		// Если исходных точек недостаточно для сглаживания или шаг некорректен
		if (line.basePointCount < 2 || stepsPerSegment <= 0) {
			// Просто копируем исходные точки в результирующий массив
			int limit = (line.basePointCount > smoothPointMAX) ? smoothPointMAX : line.basePointCount;
			for (int i = 0; i < limit; ++i) {
				line.point[i] = line.basePoint[i];
			}
			line.pointCount = limit;
			return;
		}

		// Проходим по сегментам между исходными точками basePoint
		for (int i = 0; i < line.basePointCount - 1; ++i) {
			// Формируем 4 опорные точки для Кэтмулла-Рома (с виртуальным продлением на краях)
			float4 p0 = (i == 0) ? line.basePoint[i] : line.basePoint[i - 1];
			float4 p1 = line.basePoint[i];
			float4 p2 = line.basePoint[i + 1];
			float4 p3 = (i == line.basePointCount - 2) ? line.basePoint[i + 1] : line.basePoint[i + 2];

			// Генерируем промежуточные точки внутри текущего сегмента
			for (int step = 0; step < stepsPerSegment; ++step) {
				// Защита от переполнения жестко ограниченного массива point[100]
				if (line.pointCount >= smoothPointMAX) {
					return;
				}

				float t = (float)step / (float)stepsPerSegment;
				line.point[line.pointCount] = catmullRom(p0, p1, p2, p3, t);
				line.pointCount++;
			}
		}

		// Добавляем финальную опорную точку, чтобы линия завершилась корректно
		if (line.pointCount < smoothPointMAX) {
			line.point[line.pointCount] = line.basePoint[line.basePointCount - 1];
			line.pointCount++;
		}
	}

	

	void Starline(starline& line, int stepsPerSegment) {
		line.pointCount = 0; // Сбрасываем старый результат сглаживания

		// Проходим по сегментам между исходными точками basePoint
		for (int i = 0; i < line.basePointCount - 1; ++i) {

			// Генерируем промежуточные точки внутри текущего сегмента
			for (int step = 0; step < stepsPerSegment; ++step) {
				// Защита от переполнения жестко ограниченного массива point[100]
				if (line.pointCount >= smoothPointMAX) {
					return;
				}

				float t = (float)step / (float)stepsPerSegment;
				line.point[line.pointCount] = lerp3(line.basePoint[i],line.basePoint[i+1],t);
				line.pointCount++;
			}
		}

		// Добавляем финальную опорную точку, чтобы линия завершилась корректно
		if (line.pointCount < smoothPointMAX) {
			line.point[line.pointCount] = line.basePoint[line.basePointCount - 1];
			line.pointCount++;
		}
	}


	// Функция плавной интерполяции (Smoothstep / Fade)
	inline float perlin_fade(float t) {
		return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
	}

	// Линейная интерполяция
	inline float perlin_lerp(float t, float a, float b) {
		return a + t * (b - a);
	}

	// Вычисление скалярного произведения с градиентным вектором
	inline float perlin_grad(int hash, float x, float y, float z) {
		int h = hash & 15;
		float u = h < 8 ? x : y;
		float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

	// Вспомогательная функция для получения одного скалярного значения шума
	float GetSinglePerlinNoise3D(float x, float y, float z) {
		// Таблица перестановок Перлина (повторена дважды, чтобы избежать выхода за границы при +1)
		static const int p[512] = {
			151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
			190,6,148,247,120,234,75,0,26,56,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,
			125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,
			105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,
			135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,
			82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,
			153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
			251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,
			157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,
			66,215,61,156,180,
			// Повторение массива
			151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
			190,6,148,247,120,234,75,0,26,56,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,
			125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,
			105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,
			135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,
			82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,
			153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
			251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,
			157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,
			66,215,61,156,180
		};

		int X = static_cast<int>(::std::floor(x)) & 255;
		int Y = static_cast<int>(::std::floor(y)) & 255;
		int Z = static_cast<int>(::std::floor(z)) & 255;

		x -= ::std::floor(x);
		y -= ::std::floor(y);
		z -= ::std::floor(z);

		float u = perlin_fade(x);
		float v = perlin_fade(y);
		float w = perlin_fade(z);

		int A = p[X] + Y;
		int AA = p[A] + Z;
		int AB = p[A + 1] + Z;
		int B = p[X + 1] + Y;
		int BA = p[B] + Z;
		int BB = p[B + 1] + Z;

		return perlin_lerp(w, perlin_lerp(v, perlin_lerp(u, perlin_grad(p[AA], x, y, z),
			perlin_grad(p[BA], x - 1, y, z)),
			perlin_lerp(u, perlin_grad(p[AB], x, y - 1, z),
				perlin_grad(p[BB], x - 1, y - 1, z))),
			perlin_lerp(v, perlin_lerp(u, perlin_grad(p[AA + 1], x, y, z - 1),
				perlin_grad(p[BA + 1], x - 1, y, z - 1)),
				perlin_lerp(u, perlin_grad(p[AB + 1], x, y - 1, z - 1),
					perlin_grad(p[BB + 1], x - 1, y - 1, z - 1))));
	}

	// Целевая функция, принимающая три аргумента и возвращающая XMVECTOR
	XMVECTOR GetPerlinNoiseVector3(float x, float y, float z) {
		// Смещаем координаты для каждого канала, чтобы значения X, Y и Z не дублировали друг друга
		float nx = GetSinglePerlinNoise3D(x, y, z);
		float ny = GetSinglePerlinNoise3D(x + 31.415f, y + 58.271f, z + 93.123f);
		float nz = GetSinglePerlinNoise3D(x + 115.53f, y + 213.91f, z + 351.67f);

		// Возвращаем упакованный в SIMD-регистр вектор (компонента W = 0.0f)
		return XMVectorSet(nx, ny, nz, 0.0f);
	}

	void NewLine()
	{
		currentLine++;
		currentPoint = 0;
		starLineList.lineCount = currentLine+1;
	}

	cmd(NewStar, int x, int y, int z, int rad, int r, int g, int b, int brightness)
	{
		reflect;

		currentLine++;
		currentPoint = 0;
		starLineList.lineCount = currentLine + 1;

		starLineList.line[currentLine].baseColor = float4(in.r / 100.f, in.g / 100.f, in.b / 100.f, in.brightness / 100.f);
		starLineList.line[currentLine].basePoint[0] = float4(in.x / (float)denom, in.y / (float)denom, in.z / (float)denom, in.rad / (float)denom);
		starLineList.line[currentLine].basePointCount = 1;

		starLineList.line[currentLine].point[0] = starLineList.line[currentLine].basePoint[0];
		starLineList.line[currentLine].pointCount = 1;
	}

	float4 gemini[] = {
		// Pollux Line (Right Branch)
		{  0.95f,  0.72f,  0.0f,  0.0f }, // Pollux
		{  0.58f,  0.41f,  0.0f,  1.0f }, // Wasat
		{  0.21f,  0.12f,  0.0f,  2.0f }, // Mebsuta
		{ -0.25f, -0.28f,  0.0f,  3.0f }, // Mekbuda
		{ -0.68f, -0.65f,  0.0f,  4.0f }, // Alhena
		{ -0.92f, -0.85f,  0.0f,  5.0f }, // Alzirr

		// Castor Line (Left Branch)
		{  0.88f,  0.91f,  0.0f,  6.0f }, // Castor
		{  0.45f,  0.62f,  0.0f,  7.0f }, // Kappa Gem
		{  0.12f,  0.35f,  0.0f,  8.0f }, // Upsilon Gem
		{ -0.18f,  0.08f,  0.0f,  9.0f }, // Propus
		{ -0.52f, -0.22f,  0.0f, 10.0f }, // Tejat Posterior
		{ -0.75f, -0.45f,  0.0f, 11.0f }, // Tejat Prior

		// Connecting stars
		{  0.32f,  0.18f,  0.0f, 12.0f }, // Lambda Gem
		{  0.05f,  0.55f,  0.0f, 13.0f }, // Tau Gem
		{ -0.15f,  0.42f,  0.0f, 14.0f }, // Theta Gem
		{ -0.42f,  0.22f,  0.0f, 15.0f }, // Nu Gem
		{ -0.85f, -0.12f,  0.0f, 16.0f }  // 1 Gem
	};

	void genSegment(float4 start, float4 end)
	{
		NewLine();
		int seg = distance(start,end)*20.;
		if (seg == 0) seg += 2;
		for (int k = 0; k <= seg; k++)
		{
			float4 p = lerp3(start, end, k / (float)seg);

			float rs = .02 * sin((k / (float)seg) * PI);
			p.x += getRandFloat() * rs;
			p.y += getRandFloat() * rs;
			p.z += getRandFloat() * rs;

			// Масштабируем координаты точек для игрового мира
			float scale = 600;
			p.x *= scale;
			p.y *= scale;
			p.z *= scale;

			AddPoint(p);
		}
	}

	void initPatches(float pathTime)
	{
		// init maze
		currentLine = -1;
		// 
		//-----------------------------------------
		//-----------start user space--------------
		
		int starsCount = sizeof(gemini) / sizeof(float4);
		
		srand(100);

		/*for (int i = 0; i < starsCount; i++)
		{
			gemini[i].z = getRandFloat();
		}
		
		genSegment(gemini[0], gemini[1]);
		genSegment(gemini[1], gemini[2]);
		genSegment(gemini[2], gemini[3]);
		genSegment(gemini[3], gemini[4]);
		genSegment(gemini[4], gemini[5]);
		genSegment(gemini[6], gemini[7]);
		genSegment(gemini[7], gemini[8]);
		genSegment(gemini[8], gemini[8]);
		genSegment(gemini[9], gemini[10]);
		genSegment(gemini[10], gemini[11]);
		genSegment(gemini[0], gemini[6]);
		genSegment(gemini[7], gemini[12]);
		genSegment(gemini[12], gemini[13]);
		genSegment(gemini[13], gemini[14]);
		genSegment(gemini[14], gemini[15]);
		genSegment(gemini[10], gemini[16]);
		*/
		

//#define Actual
#ifdef Actual
		NewLine();
		AddPointToLine({ 0,0,0 });
		AddPointToLine({ 100,0,0 });
		AddPointToLine({ 200,0,0 });
		AddPointToLine({ 200,110,0 });
		AddPointToLine({ 100,110,0 });
		AddPointToLine({ 0,10,0 });

		NewLine();
		AddPointToLine({ 0,0,3 });
		AddPointToLine({ 100,0,10 });
		AddPointToLine({ 200,0,13 });
		AddPointToLine({ 200,110,13 });
		AddPointToLine({ 100,110,3 });
		AddPointToLine({ 0,10,3 });
#endif

#define Tisha
#ifdef Tisha
		// СОЗВЕЗДИЕ БЛИЗНЕЦЫ
		//
		// Плоскость рисунка: X-Z
		// X = горизонталь
		// Z = вертикаль
		// Y = глубина
		//
		// Скорость: 500 ед/сек
		//
		// 1-14 = 2 минуты = 60000 единиц
		//
		// Длины участков:
		// 1-14  =  60000
		// 9-14  =  75000
		// 14-8  = 195000
		// 8-10  = 180000
		// 10-7  = 240000
		// 8-13  = 120000
		// 13-3  = 195000
		// 14-12 =  60000
		// 12-17 = 135000
		// 2-17  = 135000
		// 17-11 = 195000
		// 17-5  = 300000
		// 5-15  = 195000
		// 5-4   = 225000
		// 4-6   =  60000
		// 6-16  =  75000
		//============================================================



		//============================================================
		// ЛИНИИ
		//
		// Каждая линия точно начинается и заканчивается в координатах
		// соответствующих звёзд.
		//
		// 7 точек на каждый участок.
		// X = горизонталь рисунка
		// Z = вертикаль рисунка
		// Y = глубина
		//============================================================


		//============================================================
		// ЛИНИИ — МАСШТАБ x4 ОТНОСИТЕЛЬНО ЗВЕЗДЫ 1
		//============================================================


		//====================
		// 1 -> 14
		//====================
		NewLine();
		AddPointToLine({ 10000, 30065, 50000 });
		AddPointToLine({ 21212, -3527, 30944 });
		AddPointToLine({ 32420, -36863, 11884 });
		AddPointToLine({ 43632, -70195, -7176 });
		AddPointToLine({ 54844, -103527, -26232 });
		AddPointToLine({ 66052, -136863, -45288 });
		AddPointToLine({ 77264, -170195, -64348 });


		//====================
// 14 -> 9
// Прямая линия
//====================
		NewLine();
		AddPointToLine({ 77264, -170195, -64348 });
		AddPointToLine({ 54690, -133528, -89615 });
		AddPointToLine({ 32116,  -96861,-114882 });
		AddPointToLine({ 9542,  -60195,-140149 });
		AddPointToLine({ -13032, -23528,-165416 });
		AddPointToLine({ -35606,  13139,-190684 });
		AddPointToLine({ -59180,  49805,-215952 });



		//====================
		// 14 -> 8
		//====================
		NewLine();
		AddPointToLine({ 77264, -170195, -64348 });
		AddPointToLine({ 122328, -123527, -177004 });
		AddPointToLine({ 167388, -76863, -289660 });
		AddPointToLine({ 212452, -30195, -402320 });
		AddPointToLine({ 257516, 16473, -514976 });
		AddPointToLine({ 302576, 63137, -627632 });
		AddPointToLine({ 347640, 109805, -740288 });


		//====================
		// 8 -> 10
		//====================
		NewLine();
		AddPointToLine({ 347640, 109805, -740288 });
		AddPointToLine({ 347640, 43137, -840064 });
		AddPointToLine({ 347640, -23527, -939844 });
		AddPointToLine({ 347640, -90195, -1039624 });
		AddPointToLine({ 347640, -156863, -1139400 });
		AddPointToLine({ 347640, -223527, -1239176 });
		AddPointToLine({ 347640, -290195, -1338956 });


		//====================
		// 10 -> 7
		//====================
		NewLine();
		AddPointToLine({ 347640, -290195, -1338956 });
		AddPointToLine({ 461216, -223527, -1429816 });
		AddPointToLine({ 574796, -156863, -1520680 });
		AddPointToLine({ 688372, -90195, -1611540 });
		AddPointToLine({ 801948, -23527, -1702400 });
		AddPointToLine({ 915528, 43137, -1793264 });
		AddPointToLine({ 1029104, 109805, -1884124 });


		//====================
		// 8 -> 13
		//====================
		NewLine();
		AddPointToLine({ 347640, 109805, -740288 });
		AddPointToLine({ 399888, 56473, -769024 });
		AddPointToLine({ 452136, 3137, -797760 });
		AddPointToLine({ 504384, -50195, -826496 });
		AddPointToLine({ 556628, -103527, -855236 });
		AddPointToLine({ 608876, -156863, -883972 });
		AddPointToLine({ 661124, -210195, -912708 });


		//====================
		// 13 -> 3
		//====================
		NewLine();
		AddPointToLine({ 661124, -210195, -912708 });
		AddPointToLine({ 760528, -156863, -977320 });
		AddPointToLine({ 859928, -103527, -1041932 });
		AddPointToLine({ 959332, -50195, -1106544 });
		AddPointToLine({ 1058736, 3137, -1171152 });
		AddPointToLine({ 1158136, 56473, -1235764 });
		AddPointToLine({ 1257540, 109805, -1300376 });


		//====================
		// 14 -> 12
		//====================
		NewLine();
		AddPointToLine({ 77264, -170195, -64348 });
		AddPointToLine({ 97040, -136863, -54460 });
		AddPointToLine({ 116816, -103527, -44572 });
		AddPointToLine({ 136592, -70195, -34684 });
		AddPointToLine({ 156372, -36863, -24796 });
		AddPointToLine({ 176148, -3527, -14908 });
		AddPointToLine({ 195924, 29805, -5020 });


		//====================
		// 12 -> 17
		//====================
		NewLine();
		AddPointToLine({ 195924, 29805, -5020 });
		AddPointToLine({ 243600, -43527, 16172 });
		AddPointToLine({ 291280, -116863, 37360 });
		AddPointToLine({ 338960, -190195, 58552 });
		AddPointToLine({ 386636, -263527, 79744 });
		AddPointToLine({ 434312, -336863, 100932 });
		AddPointToLine({ 481992, -410195, 122124 });


		//====================
		// 2 -> 17
		//====================
		NewLine();
		AddPointToLine({ 190000, 109805, 310000 });
		AddPointToLine({ 238664, 23137, 286020 });
		AddPointToLine({ 287328, -63527, 262040 });
		AddPointToLine({ 335996, -150195, 238064 });
		AddPointToLine({ 384660, -236863, 214084 });
		AddPointToLine({ 433324, -323527, 190104 });
		AddPointToLine({ 481992, -410195, 122124 });


		//====================
		// 17 -> 11
		//====================
		NewLine();
		AddPointToLine({ 481992, -410195, 122124 });
		AddPointToLine({ 567404, -316863, 152016 });
		AddPointToLine({ 652816, -223527, 181912 });
		AddPointToLine({ 738228, -130195, 211808 });
		AddPointToLine({ 823640, -36863, 241700 });
		AddPointToLine({ 909052, 56473, 271592 });
		AddPointToLine({ 994464, 149805, 301488 });


		//====================
		// 17 -> 5
		//====================
		NewLine();
		AddPointToLine({ 481992, -410195, 122124 });
		AddPointToLine({ 539992, -290195, 36572 });
		AddPointToLine({ 597996, -170195, -48976 });
		AddPointToLine({ 655996, -50195, -134528 });
		AddPointToLine({ 713996, 69805, -219680 });
		AddPointToLine({ 772000, 189805, -305628 });
		AddPointToLine({ 830000, 309805, -390000 });


		//====================
		// 5 -> 15
		//====================
		NewLine();
		AddPointToLine({ 830000, 309805, -390000 });
		AddPointToLine({ 955328, 189805, -543332 });
		AddPointToLine({ 1080652, 69805, -696668 });
		AddPointToLine({ 1205980, -50195, -850000 });
		AddPointToLine({ 1331308, -170195, -1003332 });
		AddPointToLine({ 1456632, -290195, -1156668 });
		AddPointToLine({ 1581960, -410195, -1310000 });


		//====================
// 5 -> 4
//====================
		NewLine();
		AddPointToLine({ 830000, 309805, -390000 });
		AddPointToLine({ 915668, 233137, -410000 });
		AddPointToLine({ 1001332, 156473, -440000 });
		AddPointToLine({ 1087000, 79805, -480000 });
		AddPointToLine({ 1172668, 3137, -530000 });
		AddPointToLine({ 1278332, -73527, -610000 });
		AddPointToLine({ 1370000, -150195, -700000 });


		//====================
		// 4 -> 6
		//====================
		NewLine();
		AddPointToLine({ 1370000, -150195, -700000 });
		AddPointToLine({ 1376668, -130195, -680000 });
		AddPointToLine({ 1383332, -110195, -650000 });
		AddPointToLine({ 1390000, -90195, -630000 });
		AddPointToLine({ 1396668, -70195, -610000 });
		AddPointToLine({ 1403332, -50195, -600000 });
		AddPointToLine({ 1410000, -30195, -590000 });


		//====================
		// 6 -> 16
		//====================
		NewLine();
		AddPointToLine({ 1410000, -30195, -590000 });
		AddPointToLine({ 1476668, -60195, -570000 });
		AddPointToLine({ 1543332, -90195, -550000 });
		AddPointToLine({ 1610000, -120195, -530000 });
		AddPointToLine({ 1676668, -150195, -510000 });
		AddPointToLine({ 1743332, -180195, -495000 });
		AddPointToLine({ 1670000, -210195, -490000 });


		//============================================================
		// ЗВЁЗДЫ — МАСШТАБ x4 ОТНОСИТЕЛЬНО ЗВЕЗДЫ 1
		//============================================================


		// 1
		NewStar({
			.x = 10000,
			.y = 30065,
			.z = 50000,
			.rad = 6630,
			.r = 100,
			.g = 0,
			.b = 0,
			.brightness = 100
			});


		// 2
		NewStar({
			.x = 190000,
			.y = 109805,
			.z = 310000,
			.rad = 6630,
			.r = 100,
			.g = 0,
			.b = 0,
			.brightness = 100
			});


		// 3
		NewStar({
			.x = 1257540,
			.y = 109805,
			.z = -1300376,
			.rad = 6630,
			.r = 100,
			.g = 0,
			.b = 0,
			.brightness = 100
			});


		// 4
		NewStar({
			.x = 1370000,
			.y = -150195,
			.z = -700000,
			.rad = 6630,
			.r = 100,
			.g = 100,
			.b = 0,
			.brightness = 100
			});


		// 5
		NewStar({
			.x = 830000,
			.y = 309805,
			.z = -390000,
			.rad = 6630,
			.r = 0,
			.g = 100,
			.b = 0,
			.brightness = 100
			});


		// 6
		NewStar({
			.x = 1410000,
			.y = -30195,
			.z = -590000,
			.rad = 6630,
			.r = 100,
			.g = 100,
			.b = 0,
			.brightness = 100
			});


		// 7
		NewStar({
			.x = 1029104,
			.y = 109805,
			.z = -1884124,
			.rad = 6630,
			.r = 100,
			.g = 0,
			.b = 0,
			.brightness = 100
			});


		// 8
		NewStar({
			.x = 347640,
			.y = 109805,
			.z = -740288,
			.rad = 6630,
			.r = 0,
			.g = 100,
			.b = 0,
			.brightness = 100
			});


		// 9
		NewStar({
			.x = -59180,
			.y = 49805,
			.z = -215952,
			.rad = 6630,
			.r = 100,
			.g = 0,
			.b = 0,
			.brightness = 100
			});


		// 10
		NewStar({
			.x = 347640,
			.y = -290195,
			.z = -1338956,
			.rad = 6630,
			.r = 100,
			.g = 100,
			.b = 0,
			.brightness = 100
			});


		// 11
		NewStar({
			.x = 994464,
			.y = 149805,
			.z = 301488,
			.rad = 6630,
			.r = 100,
			.g = 0,
			.b = 0,
			.brightness = 100
			});


		// 12
		NewStar({
			.x = 195924,
			.y = 29805,
			.z = -5020,
			.rad = 6630,
			.r = 0,
			.g = 0,
			.b = 100,
			.brightness = 100
			});


		// 13
		NewStar({
			.x = 661124,
			.y = -210195,
			.z = -912708,
			.rad = 6630,
			.r = 100,
			.g = 100,
			.b = 0,
			.brightness = 100
			});


		// 14
		NewStar({
			.x = 77264,
			.y = -170195,
			.z = -64348,
			.rad = 6630,
			.r = 0,
			.g = 0,
			.b = 100,
			.brightness = 100
			});


		// 15
		NewStar({
			.x = 1581960,
			.y = -410195,
			.z = -1310000,
			.rad = 6630,
			.r = 100,
			.g = 0,
			.b = 0,
			.brightness = 100
			});


		// 16
		NewStar({
			.x = 1670000,
			.y = -210195,
			.z = -490000,
			.rad = 6630,
			.r = 100,
			.g = 0,
			.b = 0,
			.brightness = 100
			});


		// 17
		NewStar({
			.x = 481992,
			.y = -410195,
			.z = 122124,
			.rad = 6630,
			.r = 0,
			.g = 0,
			.b = 100,
			.brightness = 100
			});
#endif


		/*for (int i = 0; i < starsCount; i++)
		{
			// Начинаем с i + 1, чтобы не проверять i==j и не дублировать пары (j,i)
			for (int j = i + 1; j < starsCount; j++)
			{
				float4 start = gemini[i];
				float4 end = gemini[j];

				// Проверка дистанции в исходном диапазоне -1...1
				if (distance(start, end) < .75)
				{
					NewLine();
					int seg = 10;
					for (int k = 0; k <= seg; k++)
					{
						float4 p = lerp3(start, end, k / (float)seg);

						float rs = .02*sin((k / (float)seg)*PI);
						p.x += getRandFloat()*rs;
						p.y += getRandFloat()*rs;
						p.z += getRandFloat()*rs;

						// Масштабируем координаты точек для игрового мира
						float scale = 600;
						p.x *= scale;
						p.y *= scale;
						p.z *= scale;

						AddPoint(p);
					}
				}
			}
		}*/

			
		//------------end user space---------------
		//-----------------------------------------

		for (int j = 0; j < starLineList.lineCount; j++)
		{
			smoothStarline(starLineList.line[j]);
			//Starline(starLineList.line[j], 3*12. / starLineList.line[j].basePointCount);
		}

		/*pathTime /= 100.;

		for (int j = 0; j < 3; j++)
		{
			ln = j;
			SetPointCountInLine({ ln,7 });
			pt = 0;
			for (int i = 0; i < 7; i++)
			{
				float4 pos;
				float amp = 10 * 10000 * ((i+1) / (j + .3) + 4);
				//pos.x = amp * sin(i * 13 + pathTime);
				//pos.y = amp * cos(i * 14 + pathTime);
				//pos.z = amp * sin(i * 15 + pathTime);

				auto v = GetPerlinNoiseVector3(ln+i * .13 + pathTime, ln+i * .25 + pathTime, ln+i * .37 + pathTime) * amp;
				pos.x = XMVectorGetX(v);
				pos.y = XMVectorGetY(v);
				pos.z = XMVectorGetZ(v);

				SetPointPosInLine({ ln,pt++, (int)pos.x,(int)pos.y,(int)pos.z,0,10000 });

			}

			smoothStarline(starLineList.line[ln], 7);
		}


		//
		*/
	}

	float4 hero_pos;

	cmd(Maze, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		for (int i = 0; i < starLineList.lineCount; i++)
		{

			vs::maze = {
				.params = {
					.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
					.gX = gX,
					.gY = gY,
					.mode = (int)in.mode,
					.skipper = in.skipper,
					.heroPosition = hero_pos,
					.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1),
				},
			};

			vs::maze.params.particlesCount = in.count;
			vs::maze.params.basePointsCount = starLineList.line[i].pointCount;

			for (int j = 0; j < starLineList.line[i].pointCount; j++)
			{
				vs::maze.params.basePoint[j] = starLineList.line[i].point[j];
			}

			vs::maze.set();

			Drawer::NullDrawer({ 1,in.count / in.skipper });
		}


	}

	int starStencilTarget = 0;

	cmd(AllStars, int count, int skipper, pMode mode, int r, int g, int b, triMode tMode)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		if (in.tMode == triMode::on)
		{
			Culling::Set({ cullmode::front });
			DepthBuf::Mode({ depthmode::off });

			if (starStencilTarget == 2)
			{
				DepthBuf::Mode({ depthmode::off });
			}

			BlendMode::Set({
				.mode = blendmode::on,
				.op = blendop::add
				});
		}
		else
		{
			psModeSet2(in.mode);

			Culling::Set({ cullmode::off });
			DepthBuf::Mode({ depthmode::readonly });
			BlendMode::Set({
				.mode = blendmode::on,
				.op = blendop::add
				});
		}

		for (int i = 0; i < starLineList.lineCount; i++)
		{
			if (starLineList.line[i].pointCount != 1) continue;//skip all paths

			auto sd = starLineList.line[i].point[0];

			int count = 500000;

			int w = sd.w;
			if (in.tMode == triMode::on)
			{
				gX = 64;
				gY = 32;


				if (starStencilTarget == 1)
				{
					//	w *= .975;
				}

				if (starStencilTarget == 2)
				{
					//w *= 1.15;
				}

				auto sd = starLineList.line[i].point[0];
				ps::starTri.params = {
						.PosRad = float4(sd.x,sd.y,sd.z,sd.w)

				};

				ps::starTri.set();

			}

			vs::star = {
				.params = {
					.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
					.gX = gX,
					.gY = gY,
					.mode = (int)in.mode,
					.skipper = 0,
					.base_color = starLineList.line[i].baseColor,
					.PosRad = float4(sd.x,sd.y,sd.z,w),
					.triMode = (int)in.tMode
				},
			};


			vs::star.set();

			if (in.tMode == triMode::on) {
				Drawer::NullDrawerTri({ gX * gY * 2, 1 });
			}
			else
			{
				Drawer::NullDrawer({ 1,in.count / in.skipper });
			}

		}

		psModeSet(in.mode);

		Culling::Set({ cullmode::off });
		DepthBuf::Mode({ depthmode::readonly });
		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});
	}

	cmd(Rocks, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::rocks = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::rocks.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });

		
	}

	cmd(Transporter, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::transporter = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::transporter.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });

		
	}

	cmd(Islands, int count; int skipper; pMode mode; int r; int g; int b;)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::islands = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::islands.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });

		
	}

	cmd(Waterfall, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::waterfall = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::waterfall.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });

		
	}

	void DoubleStar(int count, int skipper, pMode mode)
	{
		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);

		vs::fish= {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
			},
		};

		vs::fish.set();

		Drawer::NullDrawer({ 1, (int)gX * (int)gY });
	}

	void Tree(int count, int skipper, pMode mode)
	{
		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);

		vs::tree = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
			},
		};

		vs::tree.set();

		Drawer::NullDrawer({1,(int)gX*(int)gY});
	}

	void Libra_spheres(int count, int skipper, pMode mode)
	{
		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);

		vs::libra_sph = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
			},
		};

		vs::libra_sph.set();

		Drawer::NullDrawer({ 1, (int)gX * (int)gY });
	}

	void Pillars(int count,int skipper, pMode mode)
	{
		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);

		vs::pillars = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
			},
		};

		vs::pillars.set();

		Drawer::NullDrawer({ 1, (int)gX * (int)gY });
	}

	void OuterSpace(int count, int skipper, pMode mode)
	{
		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);

		vs::space = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
			},
		};

		vs::space.set();

		Drawer::NullDrawer({1,(int)gX*(int)gY});
	}

	void NeutronStar(int count, int skipper, pMode mode)
	{
		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);

		vs::neitron_star = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
			},
		};

		vs::neitron_star.set();

		Drawer::NullDrawer({ 1, (int)gX * (int)gY });
	}

	cmd(DoubleTwo, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;
		float r = in.r / 100.f;
		float g = in.g / 100.f;
		float b = in.b / 100.f;
		float4 base_color = float4(r, g, b, 1);

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::galaxy_2 = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = base_color
			},
		};

		vs::galaxy_2.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });
		
	}

	cmd(Galaxy, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;
		float r = in.r / 100.f;
		float g = in.g / 100.f;
		float b = in.b / 100.f;
		float4 base_color = float4(r,g,b, 1);

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::galaxy = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = base_color
			},
		};

		vs::galaxy.set();

		Drawer::NullDrawer({1,(int)gX*(int)gY});
		
	}

	cmd(Libra, int quality)
	{
		reflect;

		int pillars_cnt = 1000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Tree(pillars_cnt, 1, pMode::point);
		Libra_spheres(256 * 256, 1, pMode::point);
		InsideNebula({pillars_cnt,1,pMode::point,100,252,400});
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		Galaxy({galaxy_cnt,14,pMode::point,254,220,41});

		//mid
		RenderTarget::Set({texture::pBufMid,0});
		RenderTarget::Clear({ 0,0,0,0 });
		Galaxy({ galaxy_cnt, 4, pMode::glow ,254,220,41 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Tree(pillars_cnt, 1394 / 2, pMode::glow);
		InsideNebula({pillars_cnt,1394,pMode::glow,100,202,400});
		Libra_spheres(256 * 256, 143, pMode::glow);
			OuterSpace(outerSpace_cnt, 64, pMode::glow);

		
	}

	cmd(Aquarius, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Blob({ pillars_cnt,1,pMode::point,100,252,500 });
		Islands({ pillars_cnt/2,1,pMode::point,130,112,10 });
		Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

		//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		Blob({ pillars_cnt,194,pMode::glow,100,252,600 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);

		
	}

	cmd(Crab, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Pearl({ pillars_cnt,1,pMode::point,600,252,100 });
		InsideNebula({ pillars_cnt / 2, 1, pMode::point ,1500,100,00 });
		//Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		//Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		Pearl({ pillars_cnt,194,pMode::glow,600,252,100 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);

		
	}

	cmd(LeoBigStar, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		LeoStar({ pillars_cnt,1,pMode::point,600,252,100 });
		//InsideNebula({ pillars_cnt / 2, 1, pMode::point ,1500,100,00 });
		//Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		//Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		LeoStar({ pillars_cnt,194,pMode::glow,600,252,100 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);

		
	}

	cmd(Capri, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		CapStar({ pillars_cnt,1,pMode::point,100,252,1400 });
		//InsideNebula({ pillars_cnt / 2, 1, pMode::point ,1500,100,00 });
		//Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		//Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		CapStar({ pillars_cnt,194,pMode::glow,100,252,1400 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);

		
	}

	cmd(Taurus, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Tau({ pillars_cnt,1,pMode::point,100,252,1400 });
		//InsideNebula({ pillars_cnt / 2, 1, pMode::point ,1500,100,00 });
		//Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		//Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		Tau({ pillars_cnt,194,pMode::glow,100,252,1400 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);

		
	}


	
	


	cmd(Girl, int quality, int xPos, int yPos, int zPos, int brightness, int tickness, switcher stencil)
	{
		reflect;

		//Object::Capri({ .quality = 1 });

		int pillars_cnt2 = 2000 * 1000;

		int pillars_cnt = 3725442 / in.quality;
		int outerSpace_cnt = 6853 / in.quality;
		int neutronStar_cnt = 279620 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });

		starStencilTarget = 0;
		AllStars({ 200000,1,pMode::point,1000,200,10,triMode::on });

		vrg({ pillars_cnt/2,1,pMode::point,1390,925,111 });
		Maze({ 200000,1,pMode::point,1390,925,111 });

		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//NeutronStar(neutronStar_cnt, 1, pMode::point);

		//Galaxy({ galaxy_cnt, 14, pMode::point ,100,200,300 });

		//RenderTarget::Set({ texture::pBuf,0 });
		//RenderTarget::Clear({ 0,0,0,0 });

		//call show obj

		Culling::Set({ cullmode::off });
		DepthBuf::Mode({ depthmode::off });

		//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		vrg({ pillars_cnt,94,pMode::glow,20,30,75 });
		Maze({ 200000,94,pMode::glow,20,30,75 });

		//Galaxy({ galaxy_cnt, 4, pMode::glow ,100,200,300 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });


		//Pillars(pillars_cnt, 10394, pMode::glow);
		OuterSpace(outerSpace_cnt, 64, pMode::glow);

		//------------------
		//hi
		

		
	}

	cmd(Scorpio, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		
		ScorpBall({ pillars_cnt/2,1,pMode::point,1390,925,111 });
		Nebula2({ pillars_cnt,1,pMode::point,1390,925,111 });
		//InsideNebula({ pillars_cnt , 1, pMode::point ,220,130,175 });
		//Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		//Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		
		//ScorpBall({ pillars_cnt,1,pMode::point,1390,925,111 });
		//ScorpBall({ pillars_cnt,94,pMode::glow,1,10,5 });
		Nebula2({ pillars_cnt,94,pMode::glow,20,30,75 });
		//InsideNebula({ pillars_cnt , 134, pMode::glow ,40,16,10 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);

		
	}

	cmd(Virgo, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		vrg({ pillars_cnt,1,pMode::point,1390,925,111 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

		//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		vrg({ pillars_cnt,94,pMode::glow,20,30,75 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		OuterSpace(outerSpace_cnt, 64, pMode::glow);

		
	}

	cmd(Aries, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Rocks({ pillars_cnt,1,pMode::point,1390,925,111 });
		//InsideNebula({ pillars_cnt , 1, pMode::point ,220,130,175 });
		//Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		//Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		Rocks({ pillars_cnt,194,pMode::glow,20,30,75 });
		//InsideNebula({ pillars_cnt , 134, pMode::glow ,40,16,10 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);

		
	}

	cmd(Twins, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Transporter({ pillars_cnt,1,pMode::point,1390,925,111 });
		InsideNebula({ pillars_cnt , 1, pMode::point ,220,130,175 });
		//Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		//Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		Transporter({ pillars_cnt,194,pMode::glow,2,3,7 });
		InsideNebula({ pillars_cnt , 134, pMode::glow ,10,26,40 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);

		
	}

	cmd(Fish, int quality)
	{
		reflect;

		int pillars_cnt = 3725470 / in.quality;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;
		int galaxy_cnt2 = 2182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//DoubleStar(pillars_cnt, 1, pMode::point);
		DoubleTwo({ pillars_cnt, 1, pMode::point,90,130,800 });
		//InsideNebula({ pillars_cnt / 2, 1, pMode::point ,100,200,600});
		OuterSpace(outerSpace_cnt, 1, pMode::point);

		//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		DoubleTwo({ galaxy_cnt, 25, pMode::glow, 100, 200, 300 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		DoubleTwo({ galaxy_cnt, 2, pMode::glow,20,40,160 });
		
		//InsideNebula({pillars_cnt / 2, 1394, pMode::glow, 100, 200, 600});
			OuterSpace(outerSpace_cnt, 64, pMode::glow);

		
	}

	cmd(Zenith, int quality)
	{
		reflect;

		int pillars_cnt = 3725470/2 / in.quality;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		PillarsHand(pillars_cnt, 1, pMode::point);
		InsideNebula({ pillars_cnt, 1, pMode::point,100,200,600 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);

		//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		PillarsHand(pillars_cnt, 1394/2, pMode::glow);
		InsideNebula({ pillars_cnt, 1394, pMode::glow ,100,200,600});
	//	OuterSpace(outerSpace_cnt, 64, pMode::glow);

		
	}

	cmd(Saggitarius, int quality)
	{
		reflect;

		int pillars_cnt = 3725470/in.quality;
		int outerSpace_cnt = 6853 / in.quality;
		int neutronStar_cnt = 279620 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

			Pillars(pillars_cnt,1,pMode::point);
			OuterSpace(outerSpace_cnt, 1, pMode::point);
			NeutronStar(neutronStar_cnt, 1, pMode::point);
			
			Galaxy({ galaxy_cnt, 14, pMode::point ,100,200,300 });

		//mid
			RenderTarget::Set({ texture::pBufMid,0 });
			RenderTarget::Clear({ 0,0,0,0 });


			Galaxy({ galaxy_cnt, 4, pMode::glow ,100,200,300 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

			Pillars(pillars_cnt, 10394, pMode::glow);
			OuterSpace(outerSpace_cnt, 64, pMode::glow);
			//NeutronStar(1024 * 1024, 1, pMode::glow);
			//Galaxy(182361, 4, pMode::glow);

		
	}

	cmd(CalcNormals, texture srcGeomerty, texture targetNrml)
	{
		reflect;

		RenderTarget::Set({ in.targetNrml, 0 });

		vs::quad.set();

		ps::genNormals = {

			.textures = {
				.geo = in.srcGeomerty
			},

			.samplers = {
				.sam1Filter = filter::linear,
				.sam1AddressU = addr::wrap,
				.sam1AddressV = addr::wrap
			}
		};

		ps::genNormals.set();

		Drawer::NullDrawer({ 1, 1 });
		RenderTarget::GenerateMips({});

		

	}

	cmd(Calc, texture targetGeo, texture targetNrml)
	{
		reflect;

		BlendMode::Set({ blendmode::off, blendop::add });
		Culling::Set({cullmode::off});
		RenderTarget::Set({ in.targetGeo,0 });
		DepthBuf::Mode({ depthmode::off });

		//pos
		vs::quad.set();
		ps::cat.set();
		Drawer::NullDrawer({ 1, 1 });
		RenderTarget::GenerateMips({});

		//normals
		CalcNormals({ in.targetGeo, in.targetNrml });

		
	}

}
