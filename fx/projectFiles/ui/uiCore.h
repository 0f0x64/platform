#pragma once

#include <cmath>
#include <string>
#include <vector>

// Portable UI transform math. Coordinates are NDC: (-1,-1) is the bottom-left,
// (1,1) the top-right. Rotation is in radians and is aspect-correct.
namespace GameUI
{
    struct Vec2
    {
        float x = 0.0f;
        float y = 0.0f;

        constexpr Vec2 operator+(Vec2 v) const { return {x + v.x, y + v.y}; }
        constexpr Vec2 operator-(Vec2 v) const { return {x - v.x, y - v.y}; }
        constexpr Vec2 operator*(Vec2 v) const { return {x * v.x, y * v.y}; }
        constexpr Vec2 operator*(float v) const { return {x * v, y * v}; }
    };

    struct Color
    {
        float r = 1.0f;
        float g = 1.0f;
        float b = 1.0f;
        float a = 1.0f;
    };

    enum class ScreenAspectRatio
    {
        XY,
        YX,
        XX,
        YY
    };

    struct Transform2D
    {
        Vec2 position;
        Vec2 scale{1.0f, 1.0f};
        Vec2 anchorPoint;
        Vec2 parentAnchor;
        float rotation = 0.0f;
        ScreenAspectRatio ratio = ScreenAspectRatio::XY;
    };

    inline Vec2 RotateAspect(Vec2 value, float rotation, float viewportAspect)
    {
        const float c = std::cos(rotation);
        const float s = std::sin(rotation);
        return {value.x * c - value.y * s / viewportAspect,
                value.x * viewportAspect * s + value.y * c};
    }

    struct WorldTransform2D
    {
        Vec2 position;          // Pivot, including the parent's anchor and local offset.
        Vec2 scale{1.0f, 1.0f}; // Signed half extents in NDC.
        Vec2 center;
        float rotation = 0.0f;
        float viewportAspect = 1.0f;

        Vec2 TransformPoint(Vec2 local) const
        {
            return center + RotateAspect(local * scale, rotation, viewportAspect);
        }

        bool Contains(Vec2 screenNdc) const
        {
            if (!std::isfinite(screenNdc.x) || !std::isfinite(screenNdc.y) ||
                !std::isfinite(scale.x) || !std::isfinite(scale.y) ||
                std::fabs(scale.x) <= 1.0e-8f || std::fabs(scale.y) <= 1.0e-8f)
            {
                return false;
            }

            const Vec2 local = RotateAspect(screenNdc - center, -rotation, viewportAspect);
            return std::fabs(local.x / scale.x) <= 1.0f + 1.0e-6f &&
                   std::fabs(local.y / scale.y) <= 1.0f + 1.0e-6f;
        }
    };

    inline Vec2 RatioScale(ScreenAspectRatio ratio, Vec2 parentScale, float viewportAspect)
    {
        switch (ratio)
        {
        case ScreenAspectRatio::YX:
            return {parentScale.y / viewportAspect, parentScale.x * viewportAspect};
        case ScreenAspectRatio::XX:
            return {parentScale.x, parentScale.x * viewportAspect};
        case ScreenAspectRatio::YY:
            return {parentScale.y / viewportAspect, parentScale.y};
        default:
            return parentScale;
        }
    }

    inline WorldTransform2D ResolveTransform(const Transform2D& local,
                                             const WorldTransform2D& parent, float viewportAspect)
    {
        WorldTransform2D result;
        result.viewportAspect =
            std::isfinite(viewportAspect) && viewportAspect > 0.0f ? viewportAspect : 1.0f;
        result.position =
            parent.center + RotateAspect((local.parentAnchor + local.position) * parent.scale,
                                         parent.rotation, result.viewportAspect);
        result.scale = local.scale * RatioScale(local.ratio, parent.scale, result.viewportAspect);
        result.rotation = parent.rotation + local.rotation;
        result.center = result.position + RotateAspect(local.anchorPoint * result.scale * -1.0f,
                                                       result.rotation, result.viewportAspect);
        return result;
    }

    struct UIDrawCommand
    {
        WorldTransform2D world;
        Color color;
        std::wstring text;
        float fontSize = 24.0f; // Reference pixels at viewport height 1080.
        bool textCommand = false;
    };

    using UIDrawList = std::vector<UIDrawCommand>;

} // namespace GameUI
