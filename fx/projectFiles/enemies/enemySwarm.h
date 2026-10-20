#pragma once

#include "enemy.h"
#include <vector>

namespace Enemies
{
    struct SwarmConfig
    {
        float cloudRadius = 1.2f;
        float cloudSpeed = 2.0f;
        float memberSpeed = 4.0f;
        float detectionRadius = 10.0f;
        float surroundRadius = 2.8f;
        float surroundTimeout = 3.0f;
        float chargeDuration = 2.0f;
        float cooldownDuration = 1.2f;
        float damagePerMember = 1.5f;
        float maxAttackDamage = 15.0f;

        bool IsValid() const
        {
            const float values[] = {cloudRadius,     cloudSpeed,      memberSpeed,    detectionRadius,
                                    surroundRadius,  surroundTimeout, chargeDuration, cooldownDuration,
                                    damagePerMember, maxAttackDamage};
            for (float value : values)
            {
                if (!std::isfinite(value) || value <= 0)
                {
                    return false;
                }
            }
            return true;
        }
    };

    enum class SwarmState
    {
        Idle,
        Surrounding,
        Charging,
        Cooldown,
        Dead
    };

    struct EnemySwarm
    {
        std::vector<std::size_t> members;
        float4 home{};
        float4 center{};
        SwarmConfig config;
        SwarmState state = SwarmState::Idle;
        float time = 0;
        float stateTime = 0;
        float phase = 0;

        bool IsAlive(const std::vector<Enemy>& enemies) const
        {
            for (auto index : members)
            {
                if (enemies[index].alive)
                {
                    return true;
                }
            }
            return false;
        }

        void Kill(std::vector<Enemy>& enemies)
        {
            for (auto index : members)
            {
                enemies[index].Kill();
            }
            state = SwarmState::Dead;
        }

        static float4 MovePoint(float4 position, float4 target, float step)
        {
            const float4 direction = target - position;
            const float distance = length3(direction);
            return distance <= step || distance < 0.0001f ? target : position + direction * (step / distance);
        }

        void Update(std::vector<Enemy>& enemies, collision::SphereCollider* player, AliveCreation& hero,
                    float deltaTime)
        {
            if (!IsAlive(enemies))
            {
                state = SwarmState::Dead;
                return;
            }
            time += deltaTime;
            stateTime += deltaTime;
            const bool hasTarget = player && player->isTouchable && hero.health > 0;
            const float targetDistance = hasTarget ? length3(player->position - center) : INFINITY;
            if (!hasTarget || targetDistance > config.detectionRadius * 1.5f)
            {
                ChangeState(SwarmState::Idle, enemies);
            }
            else if (state == SwarmState::Idle && targetDistance <= config.detectionRadius)
            {
                ChangeState(SwarmState::Surrounding, enemies);
            }

            if (state == SwarmState::Idle || state == SwarmState::Cooldown)
            {
                center = MovePoint(center, home, config.cloudSpeed * deltaTime);
                UpdateCloud(enemies, deltaTime);
                if (state == SwarmState::Cooldown && stateTime >= config.cooldownDuration)
                {
                    ChangeState(SwarmState::Surrounding, enemies);
                }
                return;
            }

            if (state == SwarmState::Surrounding)
            {
                center = MovePoint(center, player->position, config.cloudSpeed * deltaTime);
                std::size_t aliveCount = 0;
                for (auto index : members)
                {
                    aliveCount += enemies[index].alive;
                }
                std::size_t slot = 0;
                bool ready = true;
                bool canAttack = false;
                for (auto index : members)
                {
                    auto& enemy = enemies[index];
                    if (!enemy.alive)
                    {
                        continue;
                    }
                    const float height =
                        1.0f - 2.0f * (static_cast<float>(slot) + 0.5f) / static_cast<float>(aliveCount);
                    const float radius = sqrtf((std::max)(0.0f, 1.0f - height * height));
                    const float angle = static_cast<float>(slot++) * 2.39996323f + phase;
                    const float4 offset{cosf(angle) * radius, height, sinf(angle) * radius, 0};
                    const float4 target = player->position + offset * config.surroundRadius;
                    enemy.MoveTowards(target, config.memberSpeed, deltaTime);
                    ready = ready && length3(enemy.position - target) < 0.35f;
                    canAttack =
                        canAttack || length3(enemy.position - player->position) < enemy.attackRadius + player->radius;
                }
                if (ready || (stateTime >= config.surroundTimeout && canAttack))
                {
                    ChangeState(SwarmState::Charging, enemies);
                }
                return;
            }

            if (state == SwarmState::Charging)
            {
                const float progress = (std::min)(1.0f, stateTime / config.chargeDuration);
                float damage = 0;
                for (auto index : members)
                {
                    auto& enemy = enemies[index];
                    if (!enemy.alive)
                    {
                        continue;
                    }
                    enemy.SetMoving(false);
                    enemy.SetCharging(true, progress);
                    enemy.colorCharge = progress;
                    if (progress == 1 &&
                        length3(enemy.position - player->position) < enemy.attackRadius + player->radius)
                    {
                        damage += config.damagePerMember;
                    }
                }
                if (progress == 1)
                {
                    if (damage > 0)
                    {
                        hero.TakeDamage((std::min)(damage, config.maxAttackDamage));
                    }
                    ChangeState(SwarmState::Cooldown, enemies);
                    for (auto index : members)
                    {
                        if (enemies[index].alive)
                        {
                            enemies[index].colorCharge = 3.0f;
                        }
                    }
                }
            }
        }

      private:
        void ChangeState(SwarmState next, std::vector<Enemy>& enemies)
        {
            if (state == next)
            {
                return;
            }
            state = next;
            stateTime = 0;
            for (auto index : members)
            {
                if (enemies[index].alive)
                {
                    enemies[index].SetCharging(next == SwarmState::Charging);
                }
            }
        }

        void UpdateCloud(std::vector<Enemy>& enemies, float deltaTime)
        {
            for (std::size_t slot = 0; slot < members.size(); ++slot)
            {
                auto& enemy = enemies[members[slot]];
                if (!enemy.alive)
                {
                    continue;
                }
                const float seed = phase + static_cast<float>(slot) * 2.39996323f;
                float4 offset{sinf(time * 0.73f + seed) + 0.4f * sinf(time * 1.31f - seed),
                              sinf(time * 0.91f + seed * 1.7f),
                              cosf(time * 0.63f - seed) + 0.4f * sinf(time * 1.13f + seed), 0};
                const float magnitude = length3(offset);
                if (magnitude > 1)
                {
                    offset /= magnitude;
                }
                enemy.MoveTowards(center + offset * config.cloudRadius, config.memberSpeed, deltaTime);
            }
        }
    };
}
