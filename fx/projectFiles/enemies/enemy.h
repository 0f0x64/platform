#pragma once

#include <cmath>
#include <functional>

namespace Enemies
{
    struct Enemy
    {
        using InitializeCallback = std::function<void(Enemy&)>;
        using UpdateCallback = std::function<void(Enemy&, collision::SphereCollider*, AliveCreation&, float)>;

        collision::SphereCollider* collider = nullptr;
        collision::SphereCollider* attackCollider = nullptr;
        float4 position{};
        InitializeCallback initializeCallback;
        UpdateCallback updateCallback;
        Object::MeshInstance mesh;
        float maxHealth = 100.0f;
        float health = 100.0f;
        bool alive = true;
        float attackRadius = 4.0f;
        float contactDamage = 1.5f;
        float4 baseColor{3.0f, 0.45f, 0.15f, 1};
        float4 movementCenter{};
        float4 movementTarget{};
        float movementRadius = 0.65f;
        float movementSpeed = 2.0f;
        float chargeTime = 2.0f;
        float charge = 0;
        bool isCharging = false;
        bool isMoving = false;
        float colorCharge = 0;
        float hitFlash = 0;
        float hitFlashDecay = 4.0f;

        void Initialize()
        {
            Release();
            alive = true;
            health = maxHealth;
            collider = collision::CreateSphereCollider();
            collider->collisionGroup = collision::CollisionGroup::Enemy;
            collider->radius = movementRadius;
            attackCollider = collision::CreateSphereCollider();
            attackCollider->collisionGroup = collision::CollisionGroup::Enemy;
            attackCollider->isTouchable = false;
            attackCollider->radius = attackRadius;
            if (initializeCallback)
            {
                initializeCallback(*this);
            }
            SetPosition(position);
        }

        void InitializeAnims()
        {
            if (!mesh.asset || mesh.playStates.size() < 5)
            {
                return;
            }
            mesh.playStates[1].looped = true;
            mesh.playStates[2].looped = true;
            mesh.playStates[3].weight = 100;
            mesh.playStates[3].speed = 0;
            mesh.playStates[4].weight = 1000;
            mesh.randomSurfaceSampling = true;
            mesh.Update(0.0f);
            mesh.PlayAnimation(1);
        }

        void SetPosition(float4 value)
        {
            position = {value.x, value.y, value.z, 0};
            if (collider)
            {
                collider->position = position;
            }
            if (attackCollider)
            {
                attackCollider->position = position;
            }
        }

        void MoveTowards(float4 target, float speed, float deltaTime)
        {
            const float4 direction = target - position;
            const float distance = length3(direction);
            const float step = (std::max)(speed, 0.0f) * deltaTime;
            SetMoving(distance > 0.001f && step > 0);
            if (!isMoving)
            {
                return;
            }
            SetPosition(distance <= step ? target : position + direction * (step / distance));
            XMVECTOR forward = F2V(direction / distance);
            XMVECTOR up = XMVectorSet(0, 1, 0, 0);
            if (fabsf(XMVectorGetY(forward)) > 0.99f)
            {
                up = XMVectorSet(1, 0, 0, 0);
            }
            XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));
            up = XMVector3Cross(forward, right);
            const XMMATRIX rotation(right, up, forward, XMVectorSet(0, 0, 0, 1));
            const float blend = 1.0f - expf(-8.0f * deltaTime);
            mesh.model = LerpMatrix(mesh.model, rotation, blend);
        }

        void SetMoving(bool moving)
        {
            if (isMoving == moving)
            {
                return;
            }
            isMoving = moving;
            if (mesh.asset && mesh.playStates.size() > 2)
            {
                mesh.PlayAnimation(moving ? 2 : 1);
                mesh.StopAnimation(moving ? 1 : 2);
            }
        }

        void SetCharging(bool charging, float progress = 0)
        {
            if (mesh.asset && mesh.playStates.size() > 3)
            {
                if (charging && !isCharging)
                {
                    mesh.PlayAnimation(3);
                }
                else if (!charging && isCharging)
                {
                    mesh.StopAnimation(3);
                }
                if (charging)
                {
                    mesh.playStates[3].currentTime = progress * mesh.playStates[3].duration;
                }
            }
            isCharging = charging;
            charge = charging ? progress * chargeTime : 0;
        }

        void Update(float deltaTime, collision::SphereCollider* heroCollider, AliveCreation& hero)
        {
            if (!alive || !std::isfinite(deltaTime) || deltaTime <= 0)
            {
                return;
            }
            if (updateCallback)
            {
                updateCallback(*this, heroCollider, hero, deltaTime);
            }
            SetPosition(position);
            hitFlash = (std::max)(0.0f, hitFlash - deltaTime * hitFlashDecay);
            colorCharge = (std::max)(0.0f, colorCharge - deltaTime * 4.0f);
        }

        void TakeDamage(float amount)
        {
            if (!alive || !std::isfinite(amount) || amount <= 0)
            {
                return;
            }
            if (mesh.asset && mesh.playStates.size() > 4)
            {
                mesh.PlayAnimation(4, 0.05f);
            }
            dx11::Audio::Play("Swarm_hit", false, 1.0f);
            health = (std::max)(0.0f, health - amount);
            hitFlash = 1.0f;
            if (health == 0)
            {
                Kill();
            }
        }

        void Kill()
        {
            alive = false;
            health = 0;
            isCharging = false;
            isMoving = false;
            Release();
        }

        void Release()
        {
            ReleaseCollider(collider);
            ReleaseCollider(attackCollider);
            if (mesh.boneBuffer)
            {
                mesh.boneBuffer->Release();
                mesh.boneBuffer = nullptr;
            }
        }

      private:
        static void ReleaseCollider(collision::SphereCollider*& value)
        {
            if (value)
            {
                collision::DestroySphereCollider(value);
                delete value;
                value = nullptr;
            }
        }
    };
}
