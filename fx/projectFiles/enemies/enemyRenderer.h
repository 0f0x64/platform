#pragma once

#include "enemySystem.h"

namespace Enemies
{
	class EnemyRenderer
	{
	public:
		Object::MeshAsset* meshAsset = new Object::MeshAsset;

		void Load(EnemySystem& system)
		{
			if (loaded) return;

			//mesh_.LoadObj("..//fx//projectFiles//Sphere.glb");
			meshAsset->LoadGeometry("..//fx//projectFiles//Swarm_Retop_A_pos.glb");
			if (!meshAsset->loaded)
			{
				Log("Enemy renderer: model failed to load; enemies cannot be displayed.\n");
				return;
			}
			loaded = true;

			meshAsset->LoadAnimationFile("..//fx//projectFiles//Swarm_Retop_Idle.glb", true); // 1 Бездействие
			meshAsset->LoadAnimationFile("..//fx//projectFiles//Swarm_Retop_Move.glb", true); // 2 Движение
			meshAsset->LoadAnimationFile("..//fx//projectFiles//Swarm_Retop_Attack.glb", true); // 3 Атака
			meshAsset->LoadAnimationFile("..//fx//projectFiles//Swarm_Retop_Getting_Damage.glb", true); // 4 Получение урона

			for (Enemy& enemy : system.enemies_) {
				enemy.mesh.Attach(meshAsset);
				enemy.InitializeAnims();
			}
		}

		void RenderDepth(EnemySystem& system, const float4& cameraPos, float deltaTime)
		{
			if (!meshAsset->loaded || !system.IsInitialized()) return;
			SetPassState(true);
			for (int i = 0; i < system.enemies_.size(); i++) {
				Enemy& enemy = system.enemies_[i];
				if (enemy.mesh.asset == nullptr) {
					int a = 0;
				}
				Draw(enemy, cameraPos, true, deltaTime);
			}
		}

		void RenderColor(EnemySystem& system, const float4& cameraPos, float deltaTime)
		{
			if (!meshAsset->loaded || !system.IsInitialized()) return;
			SetPassState(false);
			for (Enemy& enemy : system.enemies_) Draw(enemy, cameraPos, false, deltaTime);
		}

	private:
		bool loaded = false;

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

		void Draw(Enemy& enemy, const float4& cameraPos, bool depthPass, float deltaTime)
		{
			if (!enemy.alive) return;
			if (enemy.movementRadius <= 0.0f) return;
			if (length(enemy.position - cameraPos) > 50.f) return; // Ограничение дистанции отрисовки (костыльное решение до frustum culling)

			constexpr float PositionUnits = 10000.0f;
			constexpr float NormalizedMeshRadius = 2.0f;
			constexpr float ZoomPercent = 100.0f;
			const int zoom = static_cast<int>(std::lround(
				(enemy.movementRadius / NormalizedMeshRadius - 1.0f) * ZoomPercent));

			//enemy.mesh.model = XMMatrixIdentity();

			// Базовый цвет
			float r = 3.0f, g = 0.45f, b = 0.15f;
			enemy.mesh.color = { r, g, b, 1.0f };
			int brightness = Brightness;

			// Вспышка — подмешиваем белый
			if (enemy.hitFlash > 0.0f)
			{
				// сдвигаем цвет к белому
				enemy.mesh.color.x = 1;
				enemy.mesh.color.y = 1;
				enemy.mesh.color.z = 1;

				// и повышаем общую яркость (это твоя константа Brightness = 27)
				brightness = static_cast<int>(Brightness * (1.0f + 3.0f * enemy.hitFlash)); // до ~4x
			}

			// Заряд перед атакой
			if (enemy.colorCharge > 0.0f) {
				float k = enemy.colorCharge;
				brightness = static_cast<int>(Brightness * (1.0f + 2.0f * k));
				k *= 4.f;

				enemy.mesh.color.x *= k;
				enemy.mesh.color.y *= k;
				enemy.mesh.color.z *= k;

			}

			Object::ShowMesh(&enemy.mesh,
				depthPass ? static_cast<int>(enemy.mesh.asset->triangleCount) : ParticleCount,
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
	};
}
