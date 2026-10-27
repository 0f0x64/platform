#pragma once

#include "starGenerationTypes.h"
#include <iterator>

namespace Object
{
    inline bool AppendStars(
        const std::vector<StarGeneration::Star>& stars, float starRadius = 7.0f, float4 color = float4{1, 1, 1, 1})
    {
        if (!std::isfinite(starRadius) || starRadius <= 0 || !std::isfinite(color.x) || !std::isfinite(color.y) ||
            !std::isfinite(color.z) || !std::isfinite(color.w) || !StarGeneration::IsValid(stars))
        {
            return false;
        }

        constexpr auto capacity = std::size(starLineList.line);
        if (starLineList.lineCount < 0 || static_cast<std::size_t>(starLineList.lineCount) > capacity ||
            stars.size() > capacity - starLineList.lineCount)
        {
            return false;
        }

        for (const auto& star : stars)
        {
            auto& line = starLineList.line[starLineList.lineCount++];
            line.baseColor = color;
            line.basePointCount = line.pointCount = 1;
            const auto& position = star.position;
            line.basePoint[0] = line.point[0] = {position.x, position.y, position.z, starRadius};
            line.upVector[0] = {0, 1, 0, 0};
            BuildLineAABB(line);
        }

        if (!stars.empty())
        {
            currentLine = starLineList.lineCount - 1;
            currentPoint = starLineList.line[currentLine].basePointCount;
        }
        return true;
    }
}
