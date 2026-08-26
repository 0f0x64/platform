#pragma once

#include "../interp.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

#include "../utils.h"

namespace gltfAnim
{
	static const int BoneLimit = 256;

	struct AnimationChannel
	{
		int joint = -1;
		ConstBuf::cgltf_animation_path_type path = ConstBuf::cgltf_animation_path_type_invalid;
		::std::vector<float> times;
		::std::vector<XMFLOAT4> values;
	};

	struct AnimationClip
	{
		::std::string name;
		float duration = 0.0f;
		::std::vector<AnimationChannel> channels;

		float weight = 1.0f;
		float realWeight = 0.0f;
		float speed = 1.0f;
		float currentTime = 0.0f;
		bool looped = false;
		bool isPlaying = false;
	};

	struct Joint
	{
		::std::string name;
		int parent = -1;
		XMFLOAT4X4 local;
		XMFLOAT4X4 global;
		XMFLOAT4X4 inverseBind;
	};

	struct Scene
	{
		::std::vector<Joint> joints;
		::std::vector<AnimationClip> animations;
		::std::vector<XMFLOAT4X4> bindLocal;
		float4 modelCenterScale = float4(0.0f, 0.0f, 0.0f, 1.0f);
		::std::string modelPath;
		::std::string animationPath;
		::std::string status;
		int currentAnimation = 0;
		//float currentTime = 0.0f;
		XMMATRIX bonePalette[BoneLimit];

		// LookAt state. Values are supplied by inputController.
		float lookYawTarget = 0.0f;
		float lookYawCurrent = 0.0f;
		float lookPitchTarget = 0.0f;
		float lookPitchCurrent = 0.0f;
		bool lookAtEnabled = true;
	};

	inline Scene scene;
	inline ID3D11Buffer* boneBuffer = nullptr;

	inline int NodeIndex(cgltf_data* data, const ConstBuf::cgltf_node* node)
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

		for (size_t i = 0; i < scene.joints.size(); ++i)
		{
			if (scene.joints[i].name == name)
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

	inline int ResolveAnimationTargetJoint(cgltf_data* data, cgltf_node* node, bool remapToCurrentSkeleton)
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
			for (size_t i = 0; i < scene.joints.size(); ++i)
			{
				if (CanonicalizeJointName(scene.joints[i].name) == canonical)
				{
					return static_cast<int>(i);
				}
			}
		}

		const int byIndex = NodeIndex(data, node);
		if (byIndex >= 0 && byIndex < static_cast<int>(scene.joints.size()))
		{
			return byIndex;
		}

		return -1;
	}

	inline XMMATRIX ReadNodeLocal(const cgltf_node& node)
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

	inline void ResolveGlobalPoseJoint(size_t idx, ::std::vector<char>& resolved, ::std::vector<char>& inStack)
	{
		if (idx >= scene.joints.size() || resolved[idx])
		{
			return;
		}

		if (inStack[idx])
		{
			scene.joints[idx].global = scene.joints[idx].local;
			resolved[idx] = 1;
			return;
		}

		inStack[idx] = 1;

		const int parentIdx = scene.joints[idx].parent;
		if (parentIdx < 0 || static_cast<size_t>(parentIdx) >= scene.joints.size())
		{
			scene.joints[idx].global = scene.joints[idx].local;
		}
		else
		{
			ResolveGlobalPoseJoint(static_cast<size_t>(parentIdx), resolved, inStack);

			const XMMATRIX parent = XMLoadFloat4x4(&scene.joints[parentIdx].global);
			const XMMATRIX local = XMLoadFloat4x4(&scene.joints[idx].local);
			XMStoreFloat4x4(&scene.joints[idx].global, local * parent);
		}

		inStack[idx] = 0;
		resolved[idx] = 1;
	}

	inline void UpdateGlobalPose()
	{
		if (scene.joints.empty())
		{
			return;
		}

		::std::vector<char> resolved(scene.joints.size(), 0);
		::std::vector<char> inStack(scene.joints.size(), 0);

		for (size_t i = 0; i < scene.joints.size(); ++i)
		{
			ResolveGlobalPoseJoint(i, resolved, inStack);
		}
	}

	inline void BuildBonePalette()
	{
		for (int i = 0; i < BoneLimit; ++i)
		{
			scene.bonePalette[i] = XMMatrixIdentity();
		}

		const size_t count = ::std::min<size_t>(scene.joints.size(), BoneLimit);
		for (size_t i = 0; i < count; ++i)
		{
			const XMMATRIX global = XMLoadFloat4x4(&scene.joints[i].global);
			const XMMATRIX inverseBind = XMLoadFloat4x4(&scene.joints[i].inverseBind);
			scene.bonePalette[i] = XMMatrixTranspose(inverseBind * global);
		}
	}

	inline void ResetToBindPose()
	{
		for (size_t i = 0; i < scene.joints.size() && i < scene.bindLocal.size(); ++i)
		{
			scene.joints[i].local = scene.bindLocal[i];
		}
		UpdateGlobalPose();
	}

	inline XMFLOAT3 ExtractTranslation(const XMFLOAT4X4& matrix)
	{
		XMVECTOR scale;
		XMVECTOR rotation;
		XMVECTOR translation;
		if (XMMatrixDecompose(&scale, &rotation, &translation, XMLoadFloat4x4(&matrix)))
		{
			return XMFLOAT3(XMVectorGetX(translation), XMVectorGetY(translation), XMVectorGetZ(translation));
		}
		return XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	inline void SetTranslation(XMFLOAT4X4& matrix, const XMFLOAT3& value)
	{
		XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
		XMVECTOR rotation = XMQuaternionIdentity();
		XMVECTOR oldTranslation;
		const XMMATRIX local = XMLoadFloat4x4(&matrix);
		XMMatrixDecompose(&scale, &rotation, &oldTranslation, local);

		XMVECTOR translation = XMVectorSet(value.x, value.y, value.z, 0.0f);
		XMStoreFloat4x4(&matrix,
			XMMatrixScalingFromVector(scale) *
			XMMatrixRotationQuaternion(rotation) *
			XMMatrixTranslationFromVector(translation));
	}

	inline int GetJointDepth(int jointIndex)
	{
		int depth = 0;
		int current = jointIndex;
		while (current >= 0 && current < static_cast<int>(scene.joints.size()))
		{
			current = scene.joints[current].parent;
			depth++;
		}
		return depth;
	}

	inline int FindRootMotionJointIndex(const AnimationClip& clip)
	{
		int bestJoint = -1;
		int bestDepth = 0x7fffffff;
		for (const AnimationChannel& channel : clip.channels)
		{
			if (channel.path != cgltf_animation_path_type_translation ||
				channel.joint < 0 ||
				channel.joint >= static_cast<int>(scene.joints.size()))
			{
				continue;
			}

			bool parentAlsoTranslated = false;
			const int parent = scene.joints[channel.joint].parent;
			for (const AnimationChannel& other : clip.channels)
			{
				if (other.path == cgltf_animation_path_type_translation && other.joint == parent)
				{
					parentAlsoTranslated = true;
					break;
				}
			}
			if (parentAlsoTranslated)
			{
				continue;
			}

			const int depth = GetJointDepth(channel.joint);
			if (depth < bestDepth)
			{
				bestDepth = depth;
				bestJoint = channel.joint;
			}
		}
		return bestJoint;
	}

	/////////////////////////////////////////////////////

	inline int FindJointByPatterns(
		const ::std::vector<Joint>& joints,
		const ::std::vector<const char*>& patterns)
	{
		for (size_t i = 0; i < joints.size(); ++i)
		{
			::std::string name = joints[i].name;

			std::transform(
				name.begin(),
				name.end(),
				name.begin(),
				[](unsigned char c)
				{
					return static_cast<char>(std::tolower(c));
				});

			for (const char* pat : patterns)
			{
				::std::string p = pat;

				std::transform(
					p.begin(),
					p.end(),
					p.begin(),
					[](unsigned char c)
					{
						return static_cast<char>(std::tolower(c));
					});

				if (name.find(p) != ::std::string::npos)
					return static_cast<int>(i);
			}
		}

		return -1;
	}

	struct LookAtConfig
	{
		int headIdx = -1;
		int neckIdx = -1;
		int spineIdx = -1;
		int hipsIdx = -1;

		float headWeight = 0.75f;
		float neckWeight = 0.55f;
		float spineWeight = 0.50f;
		float hipsWeight = 0.20f;

		float maxPitch = XM_PIDIV4;
		float maxYaw = XM_PIDIV2 * 0.85f;//было 0.75f

		bool resolved = false;

		void Resolve(const ::std::vector<Joint>& joints)
		{
			if (resolved)
				return;

			headIdx = FindJointByPatterns(
				joints,
				{
					"head",
					"голов"
				});

			neckIdx = FindJointByPatterns(
				joints,
				{
					"neck",
					"шея"
				});

			spineIdx = FindJointByPatterns(
				joints,
				{
					"spine",
					"chest",
					"torso",
					"грудь",
					"спин",
					"туловище"
				});

			hipsIdx = FindJointByPatterns(
				joints,
				{
					"hips",
					"pelvis",
					"таз",
					"корн",
					"root"
				});

			resolved = true;

			Log(
				(
					"LOOKAT joints: hips=" +
					::std::to_string(hipsIdx) +
					" spine=" +
					::std::to_string(spineIdx) +
					" neck=" +
					::std::to_string(neckIdx) +
					" head=" +
					::std::to_string(headIdx) +
					"\n"
					).c_str());
		}
	};

	inline LookAtConfig lookAtConfig;

	inline void SetLookAtYaw(float yaw)
	{
		lookAtConfig.Resolve(scene.joints);

		scene.lookYawTarget =
			std::clamp(
				yaw,
				-lookAtConfig.maxYaw,
				lookAtConfig.maxYaw);
	}

	inline void SetLookAtPitch(float pitch)
	{
		lookAtConfig.Resolve(scene.joints);

		scene.lookPitchTarget =
			std::clamp(
				pitch,
				-lookAtConfig.maxPitch,
				lookAtConfig.maxPitch);
	}

	inline void SetLookAtEnabled(bool enabled)
	{
		scene.lookAtEnabled = enabled;
	}

	inline void ResetLookAtPose()
	{
		scene.lookYawTarget = 0.0f;
		scene.lookYawCurrent = 0.0f;

		scene.lookPitchTarget = 0.0f;
		scene.lookPitchCurrent = 0.0f;
	}

	inline void ApplyLookAtRotation(float deltaTime)
	{
		if (!scene.lookAtEnabled || scene.joints.empty())
			return;

		lookAtConfig.Resolve(scene.joints);

		const float smoothSpeed = 18.0f;

		const float t =
			1.0f -
			expf(-smoothSpeed * deltaTime);


		// =========================================================
		// Плавное движение yaw
		// =========================================================

		float yawDiff =
			scene.lookYawTarget -
			scene.lookYawCurrent;

		// Кратчайший путь через -PI / +PI
		yawDiff =
			atan2f(
				sinf(yawDiff),
				cosf(yawDiff));

		scene.lookYawCurrent +=
			yawDiff * t;


		// =========================================================
		// Плавное движение pitch
		// =========================================================

		scene.lookPitchCurrent +=
			(scene.lookPitchTarget -
				scene.lookPitchCurrent) * t;


		// =========================================================
		// Ограничения
		// =========================================================

		scene.lookYawCurrent =
			std::clamp(
				scene.lookYawCurrent,
				-lookAtConfig.maxYaw,
				lookAtConfig.maxYaw);

		scene.lookPitchCurrent =
			std::clamp(
				scene.lookPitchCurrent,
				-lookAtConfig.maxPitch,
				lookAtConfig.maxPitch);


		// =========================================================
		// Применение вращения к кости
		// =========================================================

		auto ApplyToJoint =
			[&](int jointIdx, float weight)
			{
				if (jointIdx < 0 ||
					jointIdx >=
					static_cast<int>(scene.joints.size()))
				{
					return;
				}


				// Берём текущую локальную позу,
				// уже полученную после animation blending.
				XMMATRIX local =
					XMLoadFloat4x4(
						&scene.joints[jointIdx].local);


				XMVECTOR scale;
				XMVECTOR rotation;
				XMVECTOR translation;


				if (!XMMatrixDecompose(
					&scale,
					&rotation,
					&translation,
					local))
				{
					return;
				}


				const float y =
					-scene.lookYawCurrent *
					weight;

				const float p =
					scene.lookPitchCurrent *
					weight;


				// Yaw вокруг локальной Y
				XMVECTOR qYaw =
					XMQuaternionRotationAxis(
						XMVectorSet(
							0.0f,
							1.0f,
							0.0f,
							0.0f),
						y);


				// Pitch вокруг локальной X
				XMVECTOR qPitch =
					XMQuaternionRotationAxis(
						XMVectorSet(
							1.0f,
							0.0f,
							0.0f,
							0.0f),
						p);


				XMVECTOR delta =
					XMQuaternionNormalize(
						XMQuaternionMultiply(
							qYaw,
							qPitch));


				XMVECTOR newRotation =
					XMQuaternionNormalize(
						XMQuaternionMultiply(
							rotation,
							delta));


				XMMATRIX newLocal =
					XMMatrixScalingFromVector(
						scale) *
					XMMatrixRotationQuaternion(
						newRotation) *
					XMMatrixTranslationFromVector(
						translation);


				XMStoreFloat4x4(
					&scene.joints[jointIdx].local,
					newLocal);
			};


		// =========================================================
		// Цепочка:
		//
		// таз → торс → шея → голова
		// =========================================================

		ApplyToJoint(
			lookAtConfig.hipsIdx,
			lookAtConfig.hipsWeight);

		ApplyToJoint(
			lookAtConfig.spineIdx,
			lookAtConfig.spineWeight);

		ApplyToJoint(
			lookAtConfig.neckIdx,
			lookAtConfig.neckWeight);

		ApplyToJoint(
			lookAtConfig.headIdx,
			lookAtConfig.headWeight);
	}

	/////////////////////////////////////////////////////

	inline void PlayAnimation(int id, float time = 0.2f) {
		AnimationClip& clip = scene.animations[id];

		if (clip.isPlaying)
			return;

		//clip.realWeight = 0.0f;
		clip.currentTime = 0.0f;
		clip.isPlaying = true;

		interp::Animate(clip.realWeight, 1.0f, time);
	}

	inline void StopAnimation(int id, float time = 0.2f) {
		AnimationClip& clip = scene.animations[id];

		if (!clip.isPlaying)
			return;

		clip.isPlaying = false;

		interp::Animate(clip.realWeight, 0.0f, time);
	}

	inline void Update(float deltaTime)
	{
		/*if (!animPlaying) {
			scene.currentTime = 0;
			return;
		}*/

		ResetToBindPose();

		if (scene.animations.empty() || scene.joints.empty())
		{
			BuildBonePalette();
			return;
		}

		// Проверяем, есть ли хоть один играющий клип
		bool anyPlaying = false;
		for (AnimationClip& clip : scene.animations)
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
		::std::vector<XMVECTOR> accumScale(scene.joints.size(), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		::std::vector<XMVECTOR> accumRotation(scene.joints.size(), XMQuaternionIdentity());
		::std::vector<XMVECTOR> accumTranslation(scene.joints.size(), XMVectorZero());
		::std::vector<bool> jointAnimated(scene.joints.size(), false);

		::std::vector<float> jointWeightSum(scene.joints.size(), 0.0f);
		std::vector<float> jointRealWeightSum(scene.joints.size(), 0.0f);

		// Проходим по всем клипам
		for (size_t i = 0; i < scene.animations.size(); i++)
		{
			AnimationClip& clip = scene.animations[i];

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
					continue;
				}
			}

			// Для каждого joint в этом клипе
			for (size_t jointIdx = 0; jointIdx < scene.joints.size(); ++jointIdx)
			{
				// Декомпозируем текущую локальную матрицу (после ResetToBindPose)
				XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
				XMVECTOR rotation = XMQuaternionIdentity();
				XMVECTOR translation = XMVectorZero();
				XMVECTOR decompScale, decompRotation, decompTranslation;
				const XMMATRIX currentLocal = XMLoadFloat4x4(&scene.joints[jointIdx].local);
				if (XMMatrixDecompose(&decompScale, &decompRotation, &decompTranslation, currentLocal))
				{
					scale = decompScale;
					rotation = decompRotation;
					translation = decompTranslation;
				}

				bool animated = false;

				// Ищем каналы для этого joint в данном клипе
				for (const AnimationChannel& channel : clip.channels)
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

					if (channel.path == cgltf_animation_path_type_translation)
					{
						translation = XMVectorLerp(a, b, alpha);
					}
					else if (channel.path == cgltf_animation_path_type_rotation)
					{
						a = XMQuaternionNormalize(a);
						b = XMQuaternionNormalize(b);
						if (XMVectorGetX(XMQuaternionDot(a, b)) < 0.0f)
						{
							b = XMVectorNegate(b);
						}
						rotation = XMQuaternionNormalize(XMQuaternionSlerp(a, b, alpha));
					}
					else if (channel.path == cgltf_animation_path_type_scale)
					{
						//scale = XMVectorLerp(a, b, alpha);
						continue;
					}
				}

				if (animated)
				{
					// Взвешенное смешивание
					if (!jointAnimated[jointIdx])
					{
						accumScale[jointIdx] = scale;
						accumRotation[jointIdx] = rotation;
						accumTranslation[jointIdx] = translation;
						jointAnimated[jointIdx] = true;
						jointWeightSum[jointIdx] = clip.weight;
					}
					else
					{
						float blend = clip.weight / (jointWeightSum[jointIdx] + clip.weight);
						accumTranslation[jointIdx] = XMVectorLerp(accumTranslation[jointIdx], translation, blend);
						accumRotation[jointIdx] = XMQuaternionSlerp(accumRotation[jointIdx], rotation, blend);
						// Scale не смешиваем — оставляем от первого клипа
						jointWeightSum[jointIdx] += clip.weight;
					}

					jointRealWeightSum[jointIdx] += clip.realWeight * clip.weight;
				}
			}
		}

		/*Log(::std::to_string(totalWeight).c_str());
		Log(" | ");
		Log(::std::to_string(invTotalWeight).c_str());
		Log("\n");*/

		// Нормализация
		for (size_t jointIdx = 0; jointIdx < scene.joints.size(); ++jointIdx)
		{
			if (!jointAnimated[jointIdx])
			{
				continue;
			}

			float invJointWeight = jointWeightSum[jointIdx] > 0.0f ? 1.0f / jointWeightSum[jointIdx] : 1.0f;

			float blendedRealWeight = jointRealWeightSum[jointIdx] * invJointWeight;
			blendedRealWeight = clamp(blendedRealWeight, 0.0f, 1.0f);

			// Bind pose для этой кости
			XMVECTOR bindScale, bindRotation, bindTranslation;
			XMMATRIX bindLocal = XMLoadFloat4x4(&scene.bindLocal[jointIdx]);
			XMMatrixDecompose(&bindScale, &bindRotation, &bindTranslation, bindLocal);

			// Смешиваем накопленную анимационную позу с bind pose по blendedRealWeight
			XMVECTOR finalTranslation = XMVectorLerp(bindTranslation, accumTranslation[jointIdx], blendedRealWeight);
			XMVECTOR finalRotation = XMQuaternionSlerp(bindRotation, accumRotation[jointIdx], blendedRealWeight);
			XMVECTOR finalScale = XMVectorLerp(bindScale, accumScale[jointIdx], blendedRealWeight);

			// Собираем матрицу
			const XMMATRIX local = XMMatrixScalingFromVector(finalScale) *
				XMMatrixRotationQuaternion(finalRotation) *
				XMMatrixTranslationFromVector(finalTranslation);
			XMStoreFloat4x4(&scene.joints[jointIdx].local, local);
		}

		UpdateGlobalPose();
		BuildBonePalette();
	}

	inline void NextAnimation()
	{
		if (scene.animations.empty())
		{
			scene.currentAnimation = 0;
			return;
		}

		int animId = (scene.currentAnimation + 1) % static_cast<int>(scene.animations.size());

		scene.currentAnimation = animId;
		AnimationClip& clip = scene.animations[animId];
		clip.currentTime = 0.0f;
		ResetToBindPose();
		BuildBonePalette();
	}

	inline void PrevAnimation()
	{
		if (scene.animations.empty())
		{
			scene.currentAnimation = 0;
			return;
		}

		const int count = static_cast<int>(scene.animations.size());
		int animId = (scene.currentAnimation + count - 1) % count;

		scene.currentAnimation = animId;
		AnimationClip& clip = scene.animations[animId];
		clip.currentTime = 0.0f;
		ResetToBindPose();
		BuildBonePalette();
	}

	inline void SetAnimation(int index)
	{
		if (scene.animations.empty())
		{
			scene.currentAnimation = 0;
			return;
		}

		if (index < 0)
		{
			index = 0;
		}

		int animId = index % static_cast<int>(scene.animations.size());

		scene.currentAnimation = animId;
		AnimationClip& clip = scene.animations[animId];
		clip.currentTime = 0.0f;
		ResetToBindPose();
		BuildBonePalette();
	}

	inline const char* CurrentAnimationLabel()
	{
		static char label[128];
		if (scene.animations.empty())
		{
			return "Anim: none";
		}

		const int index = scene.currentAnimation % static_cast<int>(scene.animations.size());
		const char* name = scene.animations[index].name.empty() ? "unnamed" : scene.animations[index].name.c_str();
		::std::snprintf(label, sizeof(label), "Anim %d/%d: %s",
			index + 1,
			static_cast<int>(scene.animations.size()),
			name);
		return label;
	}

	inline const char* CurrentModelLabel()
	{
		static char label[192];
		const ::std::string model = BaseName(scene.modelPath.c_str());
		const ::std::string anim = BaseName(scene.animationPath.c_str());
		::std::snprintf(label, sizeof(label), "Model: %s | Anim file: %s",
			model.empty() ? "none" : model.c_str(),
			anim.empty() ? "embedded" : anim.c_str());
		return label;
	}

	inline const char* AnimationStatusLabel()
	{
		static char label[160];
		::std::snprintf(label, sizeof(label), "Clips: %d | %s",
			static_cast<int>(scene.animations.size()),
			scene.status.empty() ? "ready" : scene.status.c_str());
		return label;
	}

	inline ::std::vector<::std::string> AnimationMenu()
	{
		::std::vector<::std::string> menu;
		menu.reserve(scene.animations.size());
		for (size_t i = 0; i < scene.animations.size(); ++i)
		{
			char item[160];
			const char* name = scene.animations[i].name.empty() ? "unnamed" : scene.animations[i].name.c_str();
			::std::snprintf(item, sizeof(item), "%d. %s", static_cast<int>(i + 1), name);
			menu.push_back(item);
		}
		return menu;
	}

	inline void CreateBoneBuffer(ID3D11Device* device)
	{
		if (boneBuffer)
		{
			return;
		}

		D3D11_BUFFER_DESC desc{};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(scene.bonePalette);
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

		context->UpdateSubresource(boneBuffer, 0, nullptr, scene.bonePalette, 0, 0);
		context->VSSetConstantBuffers(4, 1, &boneBuffer);
	}

	inline void ReadSkeleton(cgltf_data* data)
	{
		scene.joints.clear();
		scene.bindLocal.clear();

		scene.joints.resize(data->nodes_count);
		scene.bindLocal.resize(data->nodes_count);

		for (cgltf_size i = 0; i < data->nodes_count; ++i)
		{
			cgltf_node& node = data->nodes[i];
			Joint& joint = scene.joints[i];
			joint.name = node.name ? node.name : "";
			joint.parent = -1;
			XMMATRIX local = ReadNodeLocal(node);
			XMStoreFloat4x4(&joint.local, local);
			XMStoreFloat4x4(&joint.global, local);
			XMStoreFloat4x4(&joint.inverseBind, XMMatrixIdentity());
			scene.bindLocal[i] = joint.local;
		}

		for (cgltf_size i = 0; i < data->nodes_count; ++i)
		{
			cgltf_node& node = data->nodes[i];
			for (cgltf_size c = 0; c < node.children_count; ++c)
			{
				const int child = NodeIndex(data, node.children[c]);
				if (child >= 0 && child < static_cast<int>(scene.joints.size()))
				{
					scene.joints[child].parent = static_cast<int>(i);
				}
			}
		}

		UpdateGlobalPose();

		for (size_t i = 0; i < scene.joints.size(); ++i)
		{
			const XMMATRIX global = XMLoadFloat4x4(&scene.joints[i].global);
			XMStoreFloat4x4(&scene.joints[i].inverseBind, XMMatrixInverse(nullptr, global));
		}

		if (data->skins_count > 0)
		{
			cgltf_skin& skin = data->skins[0];
			if (skin.inverse_bind_matrices)
			{
				for (cgltf_size i = 0; i < skin.joints_count; ++i)
				{
					const int nodeIndex = NodeIndex(data, skin.joints[i]);
					if (nodeIndex < 0 || nodeIndex >= static_cast<int>(scene.joints.size()))
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
						scene.joints[nodeIndex].inverseBind = ib;
					}
				}
			}
		}

		BuildBonePalette();
	}

	inline bool ReadAnimations(cgltf_data* data, bool replaceExisting = true, bool remapToCurrentSkeleton = false)
	{
		if (replaceExisting)
		{
			scene.animations.clear();
			scene.currentAnimation = 0;
		}

		const size_t oldCount = scene.animations.size();

		for (cgltf_size ai = 0; ai < data->animations_count; ++ai)
		{
			cgltf_animation& src = data->animations[ai];
			AnimationClip clip;
			clip.name = src.name ? src.name : "";

			for (cgltf_size ci = 0; ci < src.channels_count; ++ci)
			{
				cgltf_animation_channel& srcChannel = src.channels[ci];
				if (!srcChannel.sampler || !srcChannel.target_node)
				{
					continue;
				}

				cgltf_animation_sampler& sampler = *srcChannel.sampler;
				if (!sampler.input || !sampler.output)
				{
					continue;
				}

				AnimationChannel channel;
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

				for (cgltf_size i = 0; i < sampler.input->count; ++i)
				{
					float value = 0.0f;
					cgltf_accessor_read_float(sampler.input, i, &value, 1);
					channel.times[i] = value;
					clip.duration = (::std::max)(clip.duration, value);
				}

				const bool isRotation = channel.path == cgltf_animation_path_type_rotation;
				const bool isCubicSpline = sampler.interpolation == cgltf_interpolation_type_cubic_spline;
				channel.values.resize(channel.times.size());
				for (size_t i = 0; i < channel.times.size(); ++i)
				{
					cgltf_size sampleIndex = isCubicSpline ? static_cast<cgltf_size>(i * 3 + 1) : static_cast<cgltf_size>(i);
					if (sampleIndex >= sampler.output->count) sampleIndex = sampler.output->count - 1;

					float values[4]{ 0.0f, 0.0f, 0.0f, isRotation ? 1.0f : 0.0f };
					cgltf_accessor_read_float(sampler.output, sampleIndex, values, isRotation ? 4 : 3);
					channel.values[i] = XMFLOAT4(values[0], values[1], values[2], values[3]);
				}

				clip.channels.push_back(::std::move(channel));
			}

			if (!clip.channels.empty())
			{
				scene.animations.push_back(::std::move(clip));
			}
		}

		const bool added = scene.animations.size() > oldCount;
		if (added)
		{
			int animId = static_cast<int>(oldCount);

			scene.currentAnimation = animId;
			AnimationClip& clip = scene.animations[animId];
			clip.currentTime = 0.0f;
			ResetToBindPose();
			BuildBonePalette();
		}
		return added;
	}

	inline void FillSkinDefaults(ConstBuf::vertex& out)
	{
		out.joints = XMUINT4(0, 0, 0, 0);
		out.weights = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	}

	inline bool LoadAnimationFile(const char* path, bool remapToCurrentSkeleton = true)
	{
		cgltf_options opts{};
		cgltf_data* data = nullptr;

		if (cgltf_parse_file(&opts, path, &data) != cgltf_result_success)
			return false;

		if (cgltf_load_buffers(&opts, data, path) != cgltf_result_success)
		{
			cgltf_free(data);
			return false;
		}

		bool added = ReadAnimations(data, false, remapToCurrentSkeleton);
		cgltf_free(data);
		return added;
	}

}
