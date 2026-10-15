#pragma once

#include "starGenerationTypes.h"
#include <algorithm>
#include <cstdint>
#include <random>
#include <set>
#include <utility>
#include <vector>

namespace LineGeneration
{
    using Position = StarGeneration::Position;

    struct Segment
    {
        std::vector<Position> points;
    };

    struct Connection
    {
        std::size_t sourceStarIndex = 0;
        std::size_t targetStarIndex = 0;
        std::vector<Segment> segments;
    };

    struct Settings
    {
        int segmentCount = 3;
        int gapCount = 2;
        float gapLength = 10;
        float minSegmentLength = 1;
        float maxSegmentLength = 1000;
        float overlap = 0;
        float endBend = 0;
        int samplesPerSegment = 17;
        std::uint32_t seed = 42;

        bool IsValid() const
        {
            return segmentCount >= 1 && segmentCount <= 2000 && gapCount >= 0 && gapCount < segmentCount &&
                   samplesPerSegment >= 5 && samplesPerSegment <= 1000 && std::isfinite(gapLength) && gapLength >= 0 &&
                   (gapCount == 0 || gapLength > 0) && std::isfinite(minSegmentLength) && minSegmentLength > 0 &&
                   std::isfinite(maxSegmentLength) && maxSegmentLength >= minSegmentLength && std::isfinite(overlap) &&
                   overlap >= 0 && overlap < minSegmentLength && std::isfinite(endBend) && endBend >= 0;
        }
    };

    inline bool Generate(
        const Position& start, const Position& end, const Settings& settings, std::vector<Segment>& output)
    {
        if (!start.IsFinite() || !end.IsFinite() || !settings.IsValid())
        {
            return false;
        }

        const double dx = static_cast<double>(end.x) - start.x;
        const double dy = static_cast<double>(end.y) - start.y;
        const double dz = static_cast<double>(end.z) - start.z;
        const double distance = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (distance < 0.00001)
        {
            return false;
        }

        const double minLength = settings.minSegmentLength;
        const double maxLength = settings.maxSegmentLength;
        const int overlapCount = settings.segmentCount - 1 - settings.gapCount;
        double remainingLength = distance - settings.gapCount * static_cast<double>(settings.gapLength) +
                                 overlapCount * static_cast<double>(settings.overlap);
        if (remainingLength < settings.segmentCount * minLength || remainingLength > settings.segmentCount * maxLength)
        {
            return false;
        }

        std::mt19937 random(settings.seed);
        const auto nextRandom = [&] { return static_cast<double>(random()) / (std::mt19937::max)(); };
        std::vector<int> gapMask(settings.segmentCount - 1);
        std::fill_n(gapMask.begin(), settings.gapCount, 1);
        for (int i = static_cast<int>(gapMask.size()) - 1; i > 0; --i)
        {
            std::swap(gapMask[i], gapMask[random() % (i + 1)]);
        }

        double nx = -dz, ny = 0, nz = dx;
        if (std::abs(dx) + std::abs(dz) < distance * 0.001)
        {
            nx = 0;
            ny = -dz;
            nz = dy;
        }
        const double normalLength = std::sqrt(nx * nx + ny * ny + nz * nz);
        nx /= normalLength;
        ny /= normalLength;
        nz /= normalLength;

        std::vector<Segment> result;
        result.reserve(settings.segmentCount);
        double cursor = 0;
        for (int segmentIndex = 0; segmentIndex < settings.segmentCount; ++segmentIndex)
        {
            const int remainingSegments = settings.segmentCount - segmentIndex - 1;
            const double low = (std::max)(minLength, remainingLength - remainingSegments * maxLength);
            const double high = (std::min)(maxLength, remainingLength - remainingSegments * minLength);
            const double length = remainingSegments == 0 ? remainingLength : low + nextRandom() * (high - low);
            Segment segment;
            segment.points.reserve(settings.samplesPerSegment);
            for (int sampleIndex = 0; sampleIndex < settings.samplesPerSegment; ++sampleIndex)
            {
                const double t = static_cast<double>(sampleIndex) / (settings.samplesPerSegment - 1);
                const double along = (cursor + length * t) / distance;
                const double bend = settings.endBend * length * 4 * t * (1 - t) * (1 - 2 * t);
                Position point{static_cast<float>(start.x + dx * along + nx * bend),
                    static_cast<float>(start.y + dy * along + ny * bend),
                    static_cast<float>(start.z + dz * along + nz * bend)};
                if (!point.IsFinite() || (!segment.points.empty() && point == segment.points.back()))
                {
                    return false;
                }

                segment.points.push_back(point);
            }
            result.push_back(std::move(segment));
            remainingLength -= length;
            cursor += length;
            if (remainingSegments > 0)
            {
                cursor += gapMask[segmentIndex] ? settings.gapLength : -settings.overlap;
            }
        }
        result.front().points.front() = start;
        result.back().points.back() = end;
        output = std::move(result);
        return true;
    }

    inline bool GenerateConnections(
        const std::vector<StarGeneration::Star>& stars, const Settings& settings, std::vector<Connection>& output)
    {
        if (!StarGeneration::IsValid(stars) || !settings.IsValid())
        {
            return false;
        }

        std::set<std::pair<std::size_t, std::size_t>> edges;
        for (std::size_t starIndex = 0; starIndex < stars.size(); ++starIndex)
        {
            for (auto targetIndex : stars[starIndex].connections)
            {
                edges.emplace(std::minmax(starIndex, targetIndex));
            }
        }

        std::mt19937 random(settings.seed);
        std::vector<Connection> connections;
        connections.reserve(edges.size());
        for (const auto& [sourceIndex, targetIndex] : edges)
        {
            Connection connection{sourceIndex, targetIndex, {}};
            auto connectionSettings = settings;
            connectionSettings.seed = random();
            if (!Generate(
                    stars[sourceIndex].position, stars[targetIndex].position, connectionSettings, connection.segments))
            {
                return false;
            }

            connections.push_back(std::move(connection));
        }

        output = std::move(connections);
        return true;
    }
}
