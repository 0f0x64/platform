#pragma once

#include <cmath>
#include <cstddef>
#include <vector>

namespace StarGeneration
{
    struct Position
    {
        float x = 0;
        float y = 0;
        float z = 0;

        bool IsFinite() const
        {
            return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
        }

        bool operator==(const Position&) const = default;
    };

    struct Star
    {
        Position position;
        std::vector<std::size_t> connections;
    };

    inline bool IsValid(const std::vector<Star>& stars)
    {
        for (std::size_t starIndex = 0; starIndex < stars.size(); ++starIndex)
        {
            const auto& star = stars[starIndex];
            if (!star.position.IsFinite())
            {
                return false;
            }

            for (auto targetIndex : star.connections)
            {
                if (targetIndex >= stars.size() || targetIndex == starIndex)
                {
                    return false;
                }
            }
        }

        return true;
    }
}
