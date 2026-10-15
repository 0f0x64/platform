#pragma once

#include "starGenerationTypes.h"
#include <cstdint>
#include <random>
#include <utility>
#include <vector>

namespace StarGeneration
{
    inline bool ScatterHeights(
        const std::vector<Star>& input, float height, std::uint32_t seed, std::vector<Star>& output)
    {
        if (!std::isfinite(height) || height < 0 || !IsValid(input))
        {
            return false;
        }

        auto result = input;
        std::mt19937 random(seed);
        for (auto& star : result)
        {
            const double offset = (static_cast<double>(random()) / (std::mt19937::max)() * 2.0 - 1.0) * height;
            star.position.y = static_cast<float>(star.position.y + offset);
            if (!std::isfinite(star.position.y))
            {
                return false;
            }
        }

        output = std::move(result);
        return true;
    }
}
