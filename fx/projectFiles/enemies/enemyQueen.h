#pragma once

#include "enemySwarm.h"

namespace Enemies
{
    struct QueenConfig
    {
        float health = 350.0f;
        float radius = 1.8f;
        float patrolRadius = 2.0f;
        float patrolSpeed = 0.8f;
        float detectionRadius = 20.0f;
        float spawnRadius = 3.5f;
        float spawnInterval = 30.0f;
        float swarmDetectionRadius = 60.0f;
        std::size_t swarmsPerWave = 2;

        bool IsValid() const
        {
            const float values[] = {health,          radius,      patrolRadius,  patrolSpeed,
                                    detectionRadius, spawnRadius, spawnInterval, swarmDetectionRadius};
            for (float value : values)
            {
                if (!std::isfinite(value) || value <= 0)
                {
                    return false;
                }
            }
            return swarmsPerWave > 0 && swarmsPerWave <= 32 && swarmDetectionRadius >= detectionRadius + spawnRadius;
        }
    };

    struct EnemyQueen
    {
        QueenConfig config;
        float4 home{};
        float patrolTime = 0;
        float spawnTime = 0;
        float spawnAngle = 0;

        bool Update(Enemy& enemy, collision::SphereCollider* player, AliveCreation& hero, float deltaTime)
        {
            patrolTime = fmodf(patrolTime + deltaTime * config.patrolSpeed / config.patrolRadius, 6.28318531f);
            const float4 offset{cosf(patrolTime), sinf(patrolTime * 2) * 0.25f, sinf(patrolTime), 0};
            enemy.MoveTowards(home + offset * (config.patrolRadius * 0.9f), config.patrolSpeed, deltaTime);
            if (!player || !player->isTouchable || hero.health <= 0 ||
                length3(player->position - enemy.position) > config.detectionRadius)
            {
                spawnTime = 0;
                return false;
            }
            spawnTime -= deltaTime;
            if (spawnTime > 0)
            {
                return false;
            }
            spawnTime = config.spawnInterval;
            return true;
        }

        float4 NextSpawnPosition(float4 position)
        {
            spawnAngle = fmodf(spawnAngle + 2.39996323f, 6.28318531f);
            return position + float4{cosf(spawnAngle), 0, sinf(spawnAngle), 0} * config.spawnRadius;
        }
    };
}
