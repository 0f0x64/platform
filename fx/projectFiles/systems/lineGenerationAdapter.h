#pragma once

#include "lineGenerationSystem.h"
#include <iterator>

namespace Object
{
    namespace LineGenerationAdapter
    {
        inline bool Append(const std::vector<const LineGeneration::Segment*>& segments, float4 color)
        {
            constexpr auto capacity = std::size(starLineList.line);
            if (!std::isfinite(color.x) || !std::isfinite(color.y) || !std::isfinite(color.z) ||
                !std::isfinite(color.w) || starLineList.lineCount < 0 ||
                static_cast<std::size_t>(starLineList.lineCount) > capacity ||
                segments.size() > capacity - starLineList.lineCount)
            {
                return false;
            }

            const auto& storage = starLineList.line[0];
            for (const auto* segment : segments)
            {
                if (!segment || segment->points.size() < 2 || segment->points.size() > std::size(storage.basePoint) ||
                    segment->points.size() > std::size(storage.point))
                {
                    return false;
                }

                for (std::size_t pointIndex = 0; pointIndex < segment->points.size(); ++pointIndex)
                {
                    const auto& point = segment->points[pointIndex];
                    if (!point.IsFinite() || (pointIndex > 0 && point == segment->points[pointIndex - 1]))
                    {
                        return false;
                    }
                }
            }

            for (const auto* segment : segments)
            {
                auto& line = starLineList.line[starLineList.lineCount++];
                line.baseColor = color;
                line.basePointCount = line.pointCount = static_cast<int>(segment->points.size());
                for (int pointIndex = 0; pointIndex < line.pointCount; ++pointIndex)
                {
                    const auto& position = segment->points[pointIndex];
                    line.basePoint[pointIndex] = line.point[pointIndex] = {position.x, position.y, position.z, 0};
                    line.upVector[pointIndex] = {0, 1, 0, 0};
                }
                BuildLineAABB(line);
            }

            if (!segments.empty())
            {
                currentLine = starLineList.lineCount - 1;
                currentPoint = starLineList.line[currentLine].basePointCount;
            }
            return true;
        }
    }

    inline bool AppendConnections(
        const std::vector<LineGeneration::Connection>& connections, float4 color = float4{1, 1, 1, 1})
    {
        std::vector<const LineGeneration::Segment*> segments;
        for (const auto& connection : connections)
        {
            if (connection.segments.size() > std::size(starLineList.line) - segments.size())
            {
                return false;
            }

            for (const auto& segment : connection.segments)
            {
                segments.push_back(&segment);
            }
        }

        return LineGenerationAdapter::Append(segments, color);
    }

    inline bool AppendLineSegments(
        float4 start, float4 end, const LineGeneration::Settings& settings, float4 color = float4{1, 1, 1, 1})
    {
        std::vector<LineGeneration::Segment> generated;
        if (!LineGeneration::Generate({start.x, start.y, start.z}, {end.x, end.y, end.z}, settings, generated))
        {
            return false;
        }

        std::vector<const LineGeneration::Segment*> segments;
        segments.reserve(generated.size());
        for (const auto& segment : generated)
        {
            segments.push_back(&segment);
        }

        return LineGenerationAdapter::Append(segments, color);
    }
}
