#pragma once

#include "enemy.h"

namespace Enemies
{
	struct EnemySwarm
	{
		float midPos;
		std::vector<Enemy> swarm;

		void Initialize(const size_t count) {
			for (size_t i = 0; i < count; i++) {
				Enemy enemy = Enemy();

			}
		}

		void Update() {

		}
	};
}