#pragma once

#include "enemy.h"
#include <vector>
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
		static constexpr std::size_t Count = 100;
		static constexpr auto DefaultSeed = std::mt19937::default_seed;

		EnemySystem() = default;
		EnemySystem(const EnemySystem&) = delete;
		EnemySystem& operator=(const EnemySystem&) = delete;
		EnemySystem(EnemySystem&&) = delete;
		EnemySystem& operator=(EnemySystem&&) = delete;

		bool IsInitialized() const { return initialized_; }
		const std::vector<Enemy>& Items() const { return enemies_; }

		void Reset(float4 center, float4 right, float4 forward,
			std::mt19937::result_type seed = DefaultSeed, const SpawnConfig& config = {})
		{

			for (Enemy& e : enemies_)
				if (e.collider) { collision::DestroySphereCollider(e.collider);e.collider = nullptr; }
			enemies_.clear();
			enemies_.resize(Count);
			random_.seed(seed);
			for (std::size_t i = 0; i < Count; ++i)
			{
				Enemy& enemy = enemies_[i];
				enemy = Enemy{};
				const float lateral = (static_cast<float>(i) - (Count - 1) * 0.5f) * config.spacing;
				const float depth = config.forwardDistance + (i % 2) * config.rowSpacing;
				const float4 spawn = {
					center.x + right.x * lateral + forward.x * depth,
					center.y + right.y * lateral + forward.y * depth,
					center.z + right.z * lateral + forward.z * depth
				};
				enemy.movementRadius = config.radius > 0.0f ? config.radius : 0.0f;
				const float speed = config.baseSpeed + config.speedVariation * (i % 4);
				//enemy.movementSpeed = speed > 0.0f ? speed : 0.0f;
				enemy.initializeCallback = [spawn](Enemy& value) {
					value.position = spawn;
					value.movementCenter = spawn;
					value.movementTarget = spawn;
					};
				enemy.Initialize();
				enemy.movementTarget = RandomTarget(enemy);
				enemy.updateCallback = [this](Enemy& value, collision::SphereCollider* hc, float dt) { SwarmUpdate(value, hc, dt); };
			}
			initialized_ = true;
		}

		void ResetRandomOnLines(std::mt19937::result_type seed = DefaultSeed, const SpawnConfig& config = {})
		{
			for (Enemy& e : enemies_)
				if (e.collider) { collision::DestroySphereCollider(e.collider);e.collider = nullptr; }
			enemies_.clear();
			enemies_.resize(Count);
			random_.seed(seed);

			// Соберём валидные линии заранее
			std::vector<int> validLines;
			validLines.reserve(Object::starLineList.lineCount);
			for (int i = 0; i < Object::starLineList.lineCount; ++i)
			{
				const auto& line = Object::starLineList.line[i];
				if (line.pointCount > 1)
					validLines.push_back(i);
			}

			if (validLines.empty())
			{
				initialized_ = false;
				return;
			}

			std::uniform_int_distribution<int> lineDist(0, (int)validLines.size() - 1);

			for (std::size_t i = 0; i < Count; ++i)
			{
				Enemy& enemy = enemies_[i];
				enemy = Enemy{};

				const int lineIdx = validLines[lineDist(random_)];
				const auto& line = Object::starLineList.line[lineIdx];

				std::uniform_int_distribution<int> pointDist(0, line.pointCount - 1);
				const int pointIdx = pointDist(random_);

				const float4& p = line.point[pointIdx];
				const float4& up = line.upVector[pointIdx];

				const float spawnOffset = 0.5f;

				float4 spawn = {
					p.x + up.x * spawnOffset,
					p.y + up.y * spawnOffset,
					p.z + up.z * spawnOffset
				};

				enemy.movementRadius = config.radius > 0.0f ? config.radius : 0.0f;
				const float speed = config.baseSpeed + config.speedVariation * (i % 4);
				//enemy.movementSpeed = speed > 0.0f ? speed : 0.0f;

				enemy.initializeCallback = [spawn](Enemy& value) {
					value.position = spawn;
					value.movementCenter = spawn;
					value.movementTarget = spawn;
					};
				enemy.Initialize();
				enemy.movementTarget = RandomTarget(enemy);
				enemy.updateCallback = [this](Enemy& value, collision::SphereCollider* hc, float dt) { SwarmUpdate(value, hc, dt); };
			}
			initialized_ = true;
		}

		void Update(float deltaTime, collision::SphereCollider* heroCollider)
		{
			if (!initialized_ || !std::isfinite(deltaTime) || deltaTime <= 0.0f) return;
			for (Enemy& enemy : enemies_) enemy.Update(deltaTime, heroCollider);
		}

		// для спавна противника перед игроком.
		void SetEnemyPosition(std::size_t index, float4 position)
		{
			if (index >= Count)
				return;

			Enemy& enemy = enemies_[index];

			enemy.position = position;
			enemy.movementCenter = position;
			enemy.movementTarget = position;

			if (enemy.collider)
			{
				enemy.collider->radius = 1;
				enemy.collider->position.x = position.x;
				enemy.collider->position.y = position.y;
				enemy.collider->position.z = position.z;
				enemy.collider->isTouchable = true;
			}
		}
		void RemoveDead()
		{
			for (auto it = enemies_.begin();it != enemies_.end();)
			{
				if (!it->alive)
				{
					if (it->collider)
					{
						collision::DestroySphereCollider(it->collider);
						it->collider = nullptr;
					}
					it = enemies_.erase(it);
				}
				else ++it;
			}
		}
		Enemy* FindByCollider(collision::SphereCollider* col)
		{
			if (!col) return nullptr;
			for (Enemy& e : enemies_)
				if (e.collider == col) return &e;
			return nullptr;
		}

	private:
		float4 RandomTarget(const Enemy& enemy)
		{
			std::uniform_real_distribution<float> coordinate(-1.0f, 1.0f);
			float4 offset;
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

		float4 PointAroundPlayer(const float4& playerPos)
		{
			float4 direction = getRandomDirection();
			return playerPos + direction * 2.f;
		}

		void SwarmUpdate(Enemy& enemy, collision::SphereCollider* heroCollider, float deltaTime)
		{
			/*constexpr float ArrivalDistance = 0.01f;
			const float4 delta = {
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
			enemy.position.z += delta.z * scale;*/

			if (!enemy.alive) return;
			if (!heroCollider) return;

			constexpr float PlayerDetectDistance = 10.f;

			if (length(heroCollider->position - enemy.position) > PlayerDetectDistance) {
				if (enemy.position == enemy.movementCenter)
					return;

				enemy.movementTarget = enemy.movementCenter;

				float4 direction = enemy.movementTarget - enemy.position;
				float distance = length(direction);
				float4 step = normalize(direction) * enemy.movementSpeed * deltaTime;

				if (length(step) >= distance)
				{
					enemy.isCharging = true;
					enemy.position = enemy.movementTarget;
				}
				else {
					enemy.position += step;
				}
				return;
			}

			if (enemy.isCharging) {
				enemy.charge += deltaTime;
				if (enemy.charge >= enemy.chargeTime) {
					enemy.charge = 0.f;
					enemy.isCharging = false;

					enemy.attackCollider->position = enemy.position;
					collision::CollisionResult result =
						collision::sphere_vs_sphere(
							heroCollider->position,
							heroCollider->radius,
							enemy.collider->position,
							enemy.collider->radius
						);
					if (result.collided) {
						// TODOw
					}
				}
				return;
			}

			if (enemy.movementTarget == enemy.position) {
				enemy.movementTarget = PointAroundPlayer(heroCollider->position);
			}
				
			float4 direction = enemy.movementTarget - enemy.position;
			float distance = length(direction);

			/*while (distance <= 0.01f) {
				enemy.movementTarget = PointAroundPlayer(heroCollider->position);
				direction = enemy.movementTarget - enemy.position;
				distance = length(direction);
			}*/

			float4 step = normalize(direction) * enemy.movementSpeed * deltaTime;

			if (length(step) >= distance)
			{
				enemy.isCharging = true;
				enemy.position = enemy.movementTarget;
				enemy.movementTarget = PointAroundPlayer(heroCollider->position);
			}
			else {
				enemy.position += step;
			}
		}

		std::vector<Enemy> enemies_{};
		std::mt19937 random_{ DefaultSeed };
		bool initialized_ = false;
	};

}
