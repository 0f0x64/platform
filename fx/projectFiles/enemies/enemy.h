#pragma once

#include <functional>

namespace Enemies
{
	struct Position
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	struct Enemy
	{
		using InitializeCallback = std::function<void(Enemy&)>;
		using UpdateCallback = std::function<void(Enemy&, float)>;

		collision::SphereCollider* collider;

		Position position;
		Position movementCenter;
		Position movementTarget;
		float movementRadius = 0.65f;
		float movementSpeed = 0.2f;
		InitializeCallback initializeCallback;
		UpdateCallback updateCallback;

		void Initialize()
		{
			collider = collision::CreateSphereCollider();
			if (initializeCallback) initializeCallback(*this);
		}

		void Update(float deltaTime)
		{
			collider->position.x = position.x;
			collider->position.y = position.y;
			collider->position.z = position.z;

			if (deltaTime > 0.0f && updateCallback) updateCallback(*this, deltaTime);
		}
	};
}
