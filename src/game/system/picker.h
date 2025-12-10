/**
 * @file picker.h
 * @brief
 * @date 12/10/2025
 */

#pragma once

#include "core/base.h"
#include "game/system/coordinate.h"
#include "scene/camera.h"
#include <glm/glm.hpp>
#include <optional>

namespace RealmFortress
{
    class Picker
    {
    public:
        Picker() = default;

        std::optional<Coordinate> Pick(
            f32 mouse_x,
            f32 mouse_y,
            u32 screen_w,
            u32 screen_h,
            const Camera& camera
        ) const;

        std::optional<Coordinate> PickAtElevation(
            f32 mouse_x,
            f32 mouse_y,
            u32 screen_width,
            u32 screen_height,
            const Camera& camera,
            f32 elevation = 0.0f
        ) const;

    private:
        std::pair<glm::vec3, glm::vec3> ScreenToWorldRay(
            f32 mouse_x,
            f32 mouse_y,
            u32 screen_width,
            u32 screen_height,
            const Camera& camera
        ) const;

        std::optional<glm::vec3> RayPlaneIntersection(
            const glm::vec3& ray_origin,
            const glm::vec3& ray_direction,
            f32 plane_y
        ) const;

        Coordinate WorldToHex(const glm::vec3& world_position) const;
    };
} // namespace RealmFortress
