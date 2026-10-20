#pragma once

#include "enemySwarm.h"
#include <random>

namespace Enemies
{
    struct SpawnConfig
    {
        std::size_t swarmCount = 10;
        std::size_t minSwarmSize = 8;
        std::size_t maxSwarmSize = 12;
        std::size_t maxSwarms = 32;
        float spacing = 8.0f;
        float forwardDistance = 8.0f;
        float rowSpacing = 2.0f;
        float radius = 0.65f;
        float baseSpeed = 2.0f;
        float speedVariation = 0.1f;
        SwarmConfig swarm;

        bool IsValid() const
        {
            return minSwarmSize > 0 && maxSwarmSize >= minSwarmSize && maxSwarmSize <= 12 && swarmCount <= maxSwarms &&
                   maxSwarms > 0 && maxSwarms <= 32 && std::isfinite(spacing) && spacing >= 0 &&
                   std::isfinite(forwardDistance) && std::isfinite(rowSpacing) && std::isfinite(radius) && radius > 0 &&
                   std::isfinite(baseSpeed) && baseSpeed > 0 && std::isfinite(speedVariation) && speedVariation >= 0 &&
                   swarm.IsValid();
        }
    };

    class EnemySystem
    {
      public:
        static constexpr std::size_t Count = 100;
        static constexpr std::size_t MaxEnemies = 512;
        static constexpr auto DefaultSeed = std::mt19937::default_seed;
        std::vector<Enemy> enemies_;
        std::vector<EnemySwarm> swarms;

        EnemySystem() = default;
        EnemySystem(const EnemySystem&) = delete;
        EnemySystem& operator=(const EnemySystem&) = delete;
        EnemySystem(EnemySystem&&) = delete;
        EnemySystem& operator=(EnemySystem&&) = delete;

        ~EnemySystem()
        {
            Clear();
        }

        bool IsInitialized() const
        {
            return initialized_;
        }

        const std::vector<Enemy>& Items() const
        {
            return enemies_;
        }

        bool IsQueenAlive() const
        {
            return queenAlive_;
        }

        void SetQueenAlive(bool alive)
        {
            queenAlive_ = alive;
            if (!alive)
            {
                for (auto& swarm : swarms)
                {
                    swarm.Kill(enemies_);
                }
                RemoveDead();
            }
        }

        void SetMeshAsset(Object::MeshAsset* asset)
        {
            meshAsset_ = asset;
            for (auto& enemy : enemies_)
            {
                BindMesh(enemy);
            }
        }

        void Reset(float4 center, float4 right, float4 forward, std::mt19937::result_type seed = DefaultSeed,
                   const SpawnConfig& config = {})
        {
            BeginReset(seed, config);
            if (!config.IsValid())
            {
                return;
            }
            for (std::size_t i = 0; i < config.swarmCount; ++i)
            {
                const float lateral = (static_cast<float>(i) - (config.swarmCount - 1) * 0.5f) * config.spacing;
                const float depth = config.forwardDistance + (i % 2) * config.rowSpacing;
                SpawnSwarm(center + right * lateral + forward * depth);
            }
            initialized_ = true;
        }

        void ResetRandomOnLines(std::mt19937::result_type seed = DefaultSeed, const SpawnConfig& config = {})
        {
            BeginReset(seed, config);
            if (!config.IsValid())
            {
                return;
            }
            std::vector<int> lines;
            for (int i = 0; i < Object::starLineList.lineCount; ++i)
            {
                if (Object::starLineList.line[i].pointCount > 1)
                {
                    lines.push_back(i);
                }
            }
            if (lines.empty())
            {
                return;
            }
            std::uniform_int_distribution<std::size_t> lineIndex(0, lines.size() - 1);
            for (std::size_t i = 0; i < config.swarmCount; ++i)
            {
                const auto& line = Object::starLineList.line[lines[lineIndex(random_)]];
                std::uniform_int_distribution<int> pointIndex(0, line.pointCount - 1);
                const int point = pointIndex(random_);
                float4 up = line.upVector[point];
                const float upLength = length3(up);
                up = upLength > 0.0001f ? up / upLength : float4{0, 1, 0, 0};
                SpawnSwarm(line.point[point] + up * 0.5f);
            }
            initialized_ = true;
        }

        bool SpawnSwarm(float4 position, float detectionRadius = 0)
        {
            RemoveDead();
            if (!queenAlive_ || !config_.IsValid() || swarms.size() >= config_.maxSwarms ||
                !std::isfinite(position.x) || !std::isfinite(position.y) || !std::isfinite(position.z) ||
                !std::isfinite(detectionRadius) || detectionRadius < 0)
            {
                return false;
            }
            std::uniform_int_distribution<std::size_t> size(config_.minSwarmSize, config_.maxSwarmSize);
            const auto count = size(random_);
            if (count > MaxEnemies - enemies_.size() + freeSlots_.size())
            {
                return false;
            }
            EnemySwarm swarm;
            swarm.home = swarm.center = {position.x, position.y, position.z, 0};
            swarm.config = config_.swarm;
            if (detectionRadius > 0)
            {
                swarm.config.detectionRadius = detectionRadius;
            }
            swarm.phase = static_cast<float>(random_() % 10000) * 0.001f;
            swarm.members.reserve(count);
            for (std::size_t i = 0; i < count; ++i)
            {
                const auto index = CreateEnemy(position);
                auto& enemy = enemies_[index];
                const float angle = static_cast<float>(i) * 2.39996323f + swarm.phase;
                const float4 offset{cosf(angle), sinf(angle * 1.7f) * 0.5f, sinf(angle), 0};
                enemy.SetPosition(swarm.center + offset * (swarm.config.cloudRadius * 0.5f));
                enemy.contactDamage = swarm.config.damagePerMember;
                enemy.chargeTime = swarm.config.chargeDuration;
                swarm.members.push_back(index);
            }
            swarms.push_back(std::move(swarm));
            return true;
        }

        void Update(float deltaTime, collision::SphereCollider* heroCollider, AliveCreation& hero)
        {
            if (!initialized_ || !std::isfinite(deltaTime) || deltaTime <= 0)
            {
                return;
            }
            RemoveDead();
            for (auto& swarm : swarms)
            {
                swarm.Update(enemies_, heroCollider, hero, deltaTime);
            }
            for (auto& enemy : enemies_)
            {
                enemy.Update(deltaTime, heroCollider, hero);
            }
            RemoveDead();
        }

        void SetEnemyPosition(std::size_t index, float4 position)
        {
            if (index < enemies_.size())
            {
                enemies_[index].movementCenter = position;
                enemies_[index].movementTarget = position;
                enemies_[index].SetPosition(position);
            }
        }

        void RemoveDead()
        {
            for (auto it = swarms.begin(); it != swarms.end();)
            {
                if (it->IsAlive(enemies_))
                {
                    ++it;
                    continue;
                }
                for (auto index : it->members)
                {
                    enemies_[index].Release();
                    freeSlots_.push_back(index);
                }
                it = swarms.erase(it);
            }
        }

        Enemy* FindByCollider(collision::SphereCollider* collider)
        {
            if (!collider)
            {
                return nullptr;
            }
            for (auto& enemy : enemies_)
            {
                if (enemy.alive && enemy.collider == collider)
                {
                    return &enemy;
                }
            }
            return nullptr;
        }

      private:
        SpawnConfig config_;
        Object::MeshAsset* meshAsset_ = nullptr;
        std::vector<std::size_t> freeSlots_;
        std::mt19937 random_{DefaultSeed};
        bool initialized_ = false;
        bool queenAlive_ = true;

        void Clear()
        {
            for (auto& enemy : enemies_)
            {
                enemy.Release();
            }
            enemies_.clear();
            swarms.clear();
            freeSlots_.clear();
            initialized_ = false;
        }

        void BeginReset(std::mt19937::result_type seed, const SpawnConfig& config)
        {
            Clear();
            config_ = config;
            random_.seed(seed);
            queenAlive_ = true;
            enemies_.reserve(MaxEnemies);
            swarms.reserve(32);
        }

        std::size_t CreateEnemy(float4 position)
        {
            std::size_t index = enemies_.size();
            if (!freeSlots_.empty())
            {
                index = freeSlots_.back();
                freeSlots_.pop_back();
                enemies_[index].Release();
                enemies_[index] = Enemy{};
            }
            else
            {
                enemies_.emplace_back();
            }
            auto& enemy = enemies_[index];
            enemy.position = enemy.movementCenter = enemy.movementTarget = {position.x, position.y, position.z, 0};
            enemy.movementRadius = config_.radius;
            enemy.movementSpeed = config_.baseSpeed + config_.speedVariation * static_cast<float>(index % 4);
            enemy.Initialize();
            BindMesh(enemy);
            return index;
        }

        void BindMesh(Enemy& enemy)
        {
            if (enemy.alive && meshAsset_ && meshAsset_->loaded && enemy.mesh.asset != meshAsset_)
            {
                enemy.mesh.Attach(meshAsset_);
                enemy.InitializeAnims();
            }
        }
    };
}
