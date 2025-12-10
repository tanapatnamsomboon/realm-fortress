/**
 * @file picker.cpp
 * @brief
 * @date 12/10/2025
 */

#include "core/pch.h"
#include "picker.h"
#include "game/system/constants.h"
#include <glm/gtc/matrix_transform.hpp>

namespace RealmFortress
{
    std::optional<Coordinate> Picker::Pick(
        f32 mouse_x,
        f32 mouse_y,
        u32 screen_w,
        u32 screen_h,
        const Camera& camera
    ) const
    {
        return PickAtElevation(mouse_x, mouse_y, screen_w, screen_h, camera, 0.0f);
    }

    std::optional<Coordinate> Picker::PickAtElevation(
        f32 mouse_x,
        f32 mouse_y,
        u32 screen_width,
        u32 screen_height,
        const Camera& camera,
        f32 elevation
    ) const
    {
        auto [origin, dir] = ScreenToWorldRay(mouse_x, mouse_y, screen_width, screen_height, camera);

        auto intersection = RayPlaneIntersection(origin, dir, elevation);

        if (!intersection.has_value())
            return std::nullopt;

        return WorldToHex(intersection.value());
    }

    std::pair<glm::vec3, glm::vec3> Picker::ScreenToWorldRay(
        f32 mouse_x,
        f32 mouse_y,
        u32 screen_width,
        u32 screen_height,
        const Camera& camera
    ) const
    {
        f32 x = (2.0f * mouse_x) / screen_width - 1.0f;
        f32 y = 1.0f - (2.0f * mouse_y) / screen_height;

        glm::vec4 ray_clip(x, y, -1.0f, 1.0f);

        glm::mat4 inverse_projection = glm::inverse(camera.GetProjection());
        glm::vec4 ray_eye = inverse_projection * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

        glm::mat4 inverse_view = glm::inverse(camera.GetViewMatrix());
        glm::vec4 ray_world = inverse_view * ray_eye;

        glm::vec3 ray_dir = glm::normalize(glm::vec3(ray_world));

        glm::vec3 ray_origin = camera.GetPosition();

        return { ray_origin, ray_dir };
    }

    std::optional<glm::vec3> Picker::RayPlaneIntersection(
        const glm::vec3& ray_origin,
        const glm::vec3& ray_direction,
        f32 plane_y
    ) const
    {
        glm::vec3 plane_normal(0.0f, 1.0f, 0.0f);

        f32 denom = glm::dot(ray_direction, plane_normal);

        if (std::abs(denom) < 0.0001f)
            return std::nullopt;

        f32 t = (plane_y - ray_origin.y) / ray_direction.y;

        if (t < 0.0f)
            return std::nullopt;

        glm::vec3 intersection = ray_origin + ray_direction * t;

        return intersection;
    }

    Coordinate Picker::WorldToHex(const glm::vec3& world_position) const
    {
        f32 x = world_position.x / MODEL_SIZE;
        f32 z = world_position.z / MODEL_SIZE;

        f32 q = (SQRT3 / 3.0f * x - 1.0f / 3.0f * z);
        f32 r = (                   2.0f / 3.0f * z);

        f32 s = -q - r;

        i32 round_q = static_cast<i32>(std::round(q));
        i32 round_r = static_cast<i32>(std::round(r));
        i32 round_s = static_cast<i32>(std::round(s));

        f32 diff_q = std::abs(round_q - q);
        f32 diff_r = std::abs(round_r - r);
        f32 diff_s = std::abs(round_s - s);

        if (diff_q > diff_r && diff_q > diff_s)
        {
            round_q = -round_r - round_s;
        }
        else if (diff_r > diff_s)
        {
            round_r = -round_q - round_s;
        }

        return Coordinate(round_q, round_r);
    }
} // namespace RealmFortress
