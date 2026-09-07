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

		Position position;
		InitializeCallback initializeCallback;
		UpdateCallback updateCallback;

		void Initialize()
		{
			if (initializeCallback) initializeCallback(*this);
		}

		void Update(float deltaTime)
		{
			if (deltaTime > 0.0f && updateCallback) updateCallback(*this, deltaTime);
		}
	};
}
