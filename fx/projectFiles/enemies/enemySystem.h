#pragma once

#include "enemy.h"
#include <array>
#include <cmath>
#include <random>

namespace Enemies
{
	struct SpawnConfig
	{
		float spacing = 2.0f;
		float forwardDistance = 8.0f;
		float rowSpacing = 2.0f;
		float radius = 0.65f;
		float baseSpeed = 0.18f;
		float speedVariation = 0.03f;
	};

	class EnemySystem
	{
	public:
		static constexpr std::size_t Count = 10;
		static constexpr auto DefaultSeed = std::mt19937::default_seed;

		EnemySystem() = default;
		EnemySystem(const EnemySystem&) = delete;
		EnemySystem& operator=(const EnemySystem&) = delete;
		EnemySystem(EnemySystem&&) = delete;
		EnemySystem& operator=(EnemySystem&&) = delete;

		bool IsInitialized() const { return initialized_; }
		const std::array<Enemy, Count>& Items() const { return enemies_; }

		void Reset(Position center, Position right, Position forward,
			std::mt19937::result_type seed = DefaultSeed, const SpawnConfig& config = {})
		{
			random_.seed(seed);
			for (std::size_t i = 0; i < Count; ++i)
			{
				Enemy& enemy = enemies_[i];
				enemy = Enemy{};
				const float lateral = (static_cast<float>(i) - (Count - 1) * 0.5f) * config.spacing;
				const float depth = config.forwardDistance + (i % 2) * config.rowSpacing;
				const Position spawn = {
					center.x + right.x * lateral + forward.x * depth,
					center.y + right.y * lateral + forward.y * depth,
					center.z + right.z * lateral + forward.z * depth
				};
				enemy.movementRadius = config.radius > 0.0f ? config.radius : 0.0f;
				const float speed = config.baseSpeed + config.speedVariation * (i % 4);
				enemy.movementSpeed = speed > 0.0f ? speed : 0.0f;
				enemy.initializeCallback = [spawn](Enemy& value) {
					value.position = spawn;
					value.movementCenter = spawn;
					value.movementTarget = spawn;
				};
				enemy.Initialize();
				enemy.movementTarget = RandomTarget(enemy);
				enemy.updateCallback = [this](Enemy& value, float dt) { UpdateWander(value, dt); };
			}
			initialized_ = true;
		}

		void Update(float deltaTime)
		{
			if (!initialized_ || !std::isfinite(deltaTime) || deltaTime <= 0.0f) return;
			for (Enemy& enemy : enemies_) enemy.Update(deltaTime);
		}

	private:
		Position RandomTarget(const Enemy& enemy)
		{
			std::uniform_real_distribution<float> coordinate(-1.0f, 1.0f);
			Position offset;
			float squaredLength;
			do {
				offset = { coordinate(random_), coordinate(random_), coordinate(random_) };
				squaredLength = offset.x * offset.x + offset.y * offset.y + offset.z * offset.z;
			} while (squaredLength > 1.0f);
			return {
				enemy.movementCenter.x + offset.x * enemy.movementRadius,
				enemy.movementCenter.y + offset.y * enemy.movementRadius,
				enemy.movementCenter.z + offset.z * enemy.movementRadius
			};
		}

		void UpdateWander(Enemy& enemy, float deltaTime)
		{
			constexpr float ArrivalDistance = 0.01f;
			const Position delta = {
				enemy.movementTarget.x - enemy.position.x,
				enemy.movementTarget.y - enemy.position.y,
				enemy.movementTarget.z - enemy.position.z
			};
			const float squaredDistance = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
			if (squaredDistance < ArrivalDistance * ArrivalDistance)
			{
				enemy.movementTarget = RandomTarget(enemy);
				return;
			}
			const float distance = std::sqrt(squaredDistance);
			const float step = enemy.movementSpeed * deltaTime;
			if (step >= distance)
			{
				enemy.position = enemy.movementTarget;
				enemy.movementTarget = RandomTarget(enemy);
				return;
			}
			const float scale = step / distance;
			enemy.position.x += delta.x * scale;
			enemy.position.y += delta.y * scale;
			enemy.position.z += delta.z * scale;
		}

		std::array<Enemy, Count> enemies_{};
		std::mt19937 random_{ DefaultSeed };
		bool initialized_ = false;
	};
}
