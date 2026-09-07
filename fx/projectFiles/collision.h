namespace collision
{
    struct SphereCollider
    {
        float4 position = float4();
        float radius = 1.0f;
        bool isTouchable = true;
    };

    std::vector<SphereCollider*> colliders;

	struct CollisionResult {
		float4 position = float4();
		float4 normal = float4(0, 1, 0, 0);
		float distance = 0;
		bool collided = false;
	};

    struct RayInfo {
        float4 origin;
        float4 direction;
        bool touchableOnly;

        RayInfo()
        {
            origin = float4();
            direction = float4();
            touchableOnly = false;
        }

        RayInfo(float4 Origin, float4 Direction, bool TouchableOnly)
            : origin(Origin), direction(Direction), touchableOnly(TouchableOnly)
        {
        }
    };

    struct SphereCastInfo {
        float4 origin;
        float4 direction;
        float radius;           // Радиус сферы для SphereCast
        float maxDistance;      // Максимальная дистанция
        bool touchableOnly;

        SphereCastInfo()
        {
            origin = float4();
            direction = float4();
            radius = 0.5f;
            maxDistance = 100.0f;
            touchableOnly = false;
        }

        SphereCastInfo(float4 Origin, float4 Direction, float Radius, float MaxDistance, bool TouchableOnly = false)
            : origin(Origin), direction(Direction), radius(Radius), maxDistance(MaxDistance), touchableOnly(TouchableOnly)
        {
        }
    };

    struct RaycastResult {
        bool hit = false;
        float distance = INFINITY;
        float4 position;
        float4 normal;
        SphereCollider* collider;
        //Entity* entity;
    };

    SphereCollider* CreateSphereCollider()
    {
        SphereCollider* collider = new SphereCollider;
        colliders.push_back(collider);
        return collider;
    }

	CollisionResult sphere_vs_sphere(
		const float4 pos1, const float radius1,
		const float4 pos2, const float radius2)
	{
		CollisionResult result = CollisionResult();

		float4 vector = float4(pos1.x - pos2.x, pos1.y - pos2.y, pos1.z - pos2.z, 0);
		float magnitude = length(vector);

		if (magnitude < radius1 + radius2) {
			result.collided = true;
			result.normal = normalize(vector);

			float4 pos = result.normal;
			pos.x *= radius2;
			pos.y *= radius2;
			pos.z *= radius2;
			pos.x += pos2.x;
			pos.y += pos2.y;
			pos.z += pos2.z;

			result.position = pos;
			result.distance = (radius1 + radius2) - magnitude;
		}

		return result;
	}


    RaycastResult Raycast(const RayInfo& ray)
    {
        RaycastResult closestHit;

        for (SphereCollider* collider : colliders) {
            RaycastResult hit;

            if (!ray.touchableOnly || collider->isTouchable)
            {
                if (raycast_sphere(ray, collider, hit) && hit.distance < closestHit.distance) {
                    closestHit.hit = true;
                    closestHit.distance = hit.distance;
                    closestHit.position = hit.position;
                    closestHit.normal = hit.normal;
                    closestHit.collider = collider;
                    //closestHit.entity = entity;
                }
            }
        }

        return closestHit;
    }


    bool raycast_sphere(const RayInfo& ray, const SphereCollider* collider, RaycastResult& hit)
    {
        float4 oc = float4(ray.origin.x - collider->position.x, ray.origin.y - collider->position.y, ray.origin.z - collider->position.z, 0);

        // Квадратное уравнение для пересечения луча и сферы
        float a = dot(ray.direction, ray.direction);
        float b = 2.0f * dot(oc, ray.direction);
        float c = dot(oc, oc) - collider->radius * collider->radius;

        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            hit.hit = false;
            return false;
        }

        float sqrtDiscriminant = sqrt(discriminant);
        float t1 = (-b - sqrtDiscriminant) / (2 * a);
        float t2 = (-b + sqrtDiscriminant) / (2 * a);

        // Выбираем ближайшее положительное пересечение
        float distance = length(ray.direction);
        float t = -1.0f;
        if (t1 > 0 && t1 <= distance) {
            t = t1;
        }
        else if (t2 > 0 && t2 <= distance) {
            t = t2;
        }

        if (t < 0) {
            hit.hit = false;
            return false;
        }

        hit.hit = true;
        hit.distance = t;
        hit.position = ray.origin + ray.direction * t;
        hit.normal = normalize(hit.position - collider->position);

        return true;
    }


    RaycastResult Spherecast(const SphereCastInfo& sphereCast)
    {
        RaycastResult closestHit;
    
        // Нормализуем направление для точных вычислений
        float4 direction = normalize(sphereCast.direction);
    
        for (SphereCollider* collider : colliders) {
            RaycastResult hit;
    
            // Проверяем, активен ли коллайдер и подходит ли по фильтру
            if (!sphereCast.touchableOnly || collider->isTouchable)
            {
                // Используем новый метод проверки пересечения сферы со сферой
                if (spherecast_sphere(sphereCast, collider, hit)
                    && hit.distance < closestHit.distance) {
                    closestHit.hit = true;
                    closestHit.distance = hit.distance;
                    closestHit.position = hit.position;
                    closestHit.normal = hit.normal;
                    closestHit.collider = collider;
                    //closestHit.entity = entity;
                }
            }
        }
    
        return closestHit;
    }
    
    bool spherecast_sphere(const SphereCastInfo& sphereCast, const SphereCollider* collider, RaycastResult& hit)
    {
        // Вектор от центра сферы-цели до начала сферы-луча
        float4 oc = sphereCast.origin - collider->position;
        float4 direction = normalize(sphereCast.direction);
    
        // Суммарный радиус (сфера луча + сфера цели)
        float combinedRadius = sphereCast.radius + collider->radius;
        float maxDistance = sphereCast.maxDistance;
    
        // Квадратное уравнение для пересечения сферы и луча с учетом радиуса сферы-луча
        float a = dot(direction, direction); // Всегда 1 если нормализовано
        float b = 2.0f * dot(oc, direction);
        float c = dot(oc, oc) - combinedRadius * combinedRadius;
    
        float discriminant = b * b - 4 * a * c;
    
        if (discriminant < 0) {
            hit.hit = false;
            return false;
        }
    
        float sqrtDiscriminant = sqrt(discriminant);
        float t1 = (-b - sqrtDiscriminant) / (2 * a);
        float t2 = (-b + sqrtDiscriminant) / (2 * a);
    
        // Выбираем ближайшее положительное пересечение, но не дальше maxDistance
        float t = -1.0f;
        if (t1 > 0 && t1 <= maxDistance) {
            t = t1;
        }
        else if (t2 > 0 && t2 <= maxDistance) {
            t = t2;
        }
    
        if (t < 0) {
            hit.hit = false;
            return false;
        }
        // Вычисляем точку пересечения
        float4 hitPoint = sphereCast.origin + direction * t;
    
        // Вычисляем нормаль (от центра цели к точке попадания)
        float4 normal = normalize(hitPoint - collider->position);
    
        hit.hit = true;
        hit.distance = t;
        hit.position = hitPoint;
        hit.normal = normal;
    
        return true;
    }
}