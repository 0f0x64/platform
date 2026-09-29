#pragma once

#include "enemySystem.h"

namespace Enemies
{
	class EnemyRenderer
	{
	public:
		void Load()
		{
			//mesh_.LoadObj("..//fx//projectFiles//Sphere.glb");
			mesh_.LoadObj("..//fx//projectFiles//Swarm_idle.glb");
			if (!mesh_.loaded)
			{
				Log("Enemy renderer: model failed to load; enemies cannot be displayed.\n");
				return;
			}
			mesh_.randomSurfaceSampling = true;
			mesh_.Update(0.0f);
		}

		void RenderDepth(const EnemySystem& system, const float4& cameraPos, float deltaTime)
		{
			if (!mesh_.loaded || !system.IsInitialized()) return;
			SetPassState(true);
			for (const Enemy& enemy : system.Items()) Draw(enemy, cameraPos, true, deltaTime);
		}

		void RenderColor(const EnemySystem& system, const float4& cameraPos, float deltaTime)
		{
			if (!mesh_.loaded || !system.IsInitialized()) return;
			SetPassState(false);
			for (const Enemy& enemy : system.Items()) Draw(enemy, cameraPos, false, deltaTime);
		}

	private:
		void SetPassState(bool depthPass)
		{
			RenderTarget::Set({ texture::pBuf, 0 });
			InputAsm::Set({ topology::triList });
			if (depthPass)
			{
				DepthBuf::Mode({ depthmode::on });
				BlendMode::Set({ blendmode::off, blendop::add });
			}
			else
			{
				DepthBuf::Mode({ depthmode::readonly });
				BlendMode::Set({ blendmode::on, blendop::add });
			}
			Culling::Set({ cullmode::off });
		}

		void Draw(const Enemy& enemy, const float4& cameraPos, bool depthPass, float deltaTime)
		{
			if (!enemy.alive) return;
			if (enemy.movementRadius <= 0.0f) return;
			if (length(enemy.position - cameraPos) > 50.f) return; // Ограничение дистанции отрисовки (костыльное решение до frustum culling)

			constexpr float PositionUnits = 10000.0f;
			constexpr float NormalizedMeshRadius = 2.0f;
			constexpr float ZoomPercent = 100.0f;
			const int zoom = static_cast<int>(std::lround(
				(enemy.movementRadius / NormalizedMeshRadius - 1.0f) * ZoomPercent));

			mesh_.model = XMMatrixIdentity();

			// Базовый цвет
			float r = 3.0f, g = 0.45f, b = 0.15f;
			mesh_.color = { r, g, b, 1.0f };
			int brightness = Brightness;

			// Вспышка — подмешиваем белый
			if (enemy.hitFlash > 0.0f)
			{
				// сдвигаем цвет к белому
				mesh_.color.x = 1;
				mesh_.color.y = 1;
				mesh_.color.z = 1;

				// и повышаем общую яркость (это твоя константа Brightness = 27)
				brightness = static_cast<int>(Brightness * (1.0f + 3.0f * enemy.hitFlash)); // до ~4x
			}

			// Заряд перед атакой
			if (enemy.colorCharge > 0.0f) {
				float k = enemy.colorCharge;
				brightness = static_cast<int>(Brightness * (1.0f + 2.0f * k));
				k *= 4.f;

				mesh_.color.x *= k;
				mesh_.color.y *= k;
				mesh_.color.z *= k;

			}

			Object::ShowMesh(&mesh_,
				depthPass ? static_cast<int>(mesh_.triangleCount) : ParticleCount,
				1, Object::pMode::point,
				100, 100, 100,
				depthPass ? Object::triMode::on : Object::triMode::off,
				static_cast<int>(enemy.position.x * PositionUnits),
				static_cast<int>(enemy.position.y * PositionUnits),
				static_cast<int>(enemy.position.z * PositionUnits),
				brightness,          // <-- сюда
				Thickness, zoom, 0, 100, deltaTime);
		}

		static constexpr int ParticleCount = 50000;
		static constexpr int Brightness = 27;
		static constexpr int Thickness = 4;
		Object::mesh mesh_;
	};
}
