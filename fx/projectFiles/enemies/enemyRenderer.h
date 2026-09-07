#pragma once

#include "enemySystem.h"

namespace Enemies
{
	class EnemyRenderer
	{
	public:
		void Load()
		{
			mesh_.LoadObj("..//fx//projectFiles//Sphere.glb");
			if (!mesh_.loaded)
			{
				Log("Enemy renderer: Sphere.glb failed to load; enemies cannot be displayed.\n");
				return;
			}
			mesh_.Update(0.0f);
		}

		void RenderDepth(const EnemySystem& system)
		{
			if (!mesh_.loaded || !system.IsInitialized()) return;
			SetPassState(true);
			for (const Enemy& enemy : system.Items()) Draw(enemy, true);
		}

		void RenderColor(const EnemySystem& system)
		{
			if (!mesh_.loaded || !system.IsInitialized()) return;
			SetPassState(false);
			for (const Enemy& enemy : system.Items()) Draw(enemy, false);
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

		void Draw(const Enemy& enemy, bool depthPass)
		{
			if (enemy.movementRadius <= 0.0f) return;
			constexpr float PositionUnits = 10000.0f;
			constexpr float NormalizedMeshRadius = 2.0f;
			constexpr float ZoomPercent = 100.0f;
			const int zoom = static_cast<int>(std::lround(
				(enemy.movementRadius / NormalizedMeshRadius - 1.0f) * ZoomPercent));
			Object::MeshDrawOptions options;
			options.model = XMMatrixIdentity();
			options.tint = { 3.0f, 0.45f, 0.15f, 1.0f };
			options.advanceAnimation = false;
			Object::ShowMesh(&mesh_, depthPass ? static_cast<int>(mesh_.triangleCount) : ParticleCount,
				1, Object::pMode::point, 100, 100, 100, depthPass ? Object::triMode::on : Object::triMode::off,
				static_cast<int>(enemy.position.x * PositionUnits),
				static_cast<int>(enemy.position.y * PositionUnits),
				static_cast<int>(enemy.position.z * PositionUnits),
				Brightness, Thickness, zoom, 0, 100, &options);
		}

		static constexpr int ParticleCount = 50000;
		static constexpr int Brightness = 27;
		static constexpr int Thickness = 4;
		Object::mesh mesh_;
	};
}
