#pragma once

#include <functional>

namespace Enemies
{
	struct Enemy
	{
		using InitializeCallback = std::function<void(Enemy&)>;
		using UpdateCallback = std::function<void(Enemy&, collision::SphereCollider*, float)>;

		collision::SphereCollider* collider;

		float4 position;
		float4 movementCenter;
		float4 movementTarget;
		float movementRadius = 0.65f;
		float movementSpeed = 0.2f;
		InitializeCallback initializeCallback;
		UpdateCallback updateCallback;

		// === HP ===
		float maxHealth = 100.0f;
		float health = 100.0f;
		bool  alive = true;

		// === Эффект вспышки при попадании ===
		float hitFlash = 0.0f;   // 0..1, гаснет со временем
		float hitFlashDecay = 4.0f;

		void Initialize()
		{
			collider = collision::CreateSphereCollider();
			collider->collisionGroup = collision::CollisionGroup::Enemy;

			if (initializeCallback) initializeCallback(*this);
		}

		void Update(float deltaTime, collision::SphereCollider* heroCollider)
		{
			if (deltaTime > 0.0f && updateCallback) updateCallback(*this, heroCollider, deltaTime);

			if (collider) {
				collider->position.x = position.x;
				collider->position.y = position.y;
				collider->position.z = position.z;
			}

			if (hitFlash > 0.0f)
			{
				hitFlash -= deltaTime * hitFlashDecay;
				if (hitFlash < 0.0f) hitFlash = 0.0f;
			}
		}

		void TakeDamage(float amount)
		{
			if (!alive || amount <= 0.0f) return;

			dx11::Audio::Play("Swarm_hit", false, 1.0f);

			health -= amount;
			hitFlash = 10.0f;              // запускаем вспышку

			if (health <= 0.0f)
			{
				health = 0.0f;
				alive = false;
				if (collider) {
					collision::DestroySphereCollider(collider);
					collider = nullptr;
				}
			}
		}
	};
}
