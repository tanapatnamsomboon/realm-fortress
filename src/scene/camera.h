/**
 * @file camera.h
 * @brief
 * @date 11/21/2025
 */

#pragma once

#include "core/base.h"
#include <glm/glm.hpp>

namespace RealmFortress
{
    /**
     * @class Camera
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Camera
    {
    public:
        Camera() = default;
        explicit Camera(const glm::mat4& projection)
            : mProjection(projection)
        {
        }

        const glm::vec3& GetPosition() const { return mPosition; }
        const glm::mat4& GetProjection() const { return mProjection; }
        const glm::mat4& GetViewMatrix() const { return mViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return mViewProjectionMatrix; }

    protected:
        glm::vec3 mPosition{ 0.0f, 0.0f, 0.0f };
        glm::mat4 mProjection{ 1.0f };
        glm::mat4 mViewMatrix{ 1.0f };
        glm::mat4 mViewProjectionMatrix{ 1.0f };
    };

    class OrthographicCamera final : public Camera
    {
    public:
        OrthographicCamera() = default;
        OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top);

        void SetProjection(f32 left, f32 right, f32 bottom, f32 top);

        void SetPosition(const glm::vec3& position) { mPosition = position; RecalculateViewMatrix(); }

        f32 GetRotation() const { return mRotation; }
        void SetRotation(f32 rotation) { mRotation = rotation; RecalculateViewMatrix(); }

    private:
        void RecalculateViewMatrix();

        f32 mRotation = 0.0f;
    };

    class PerspectiveCamera final : public Camera
    {
    public:
        PerspectiveCamera() = default;
        PerspectiveCamera(f32 fov, f32 aspect_ratio, f32 near_clip, f32 far_clip);

        void SetProjection(f32 fov, f32 aspect_ratio, f32 near_clip, f32 far_clip);

        void SetPosition(const glm::vec3& position) { mPosition = position; RecalculateViewMatrix(); }
        void SetRotation(const glm::vec3& rotation) { mRotation = rotation; RecalculateViewMatrix(); }

        const glm::vec3& GetRotation() const { return mRotation; }

        glm::vec3 GetForward() const;
        glm::vec3 GetRight() const;
        glm::vec3 GetUp() const;

    private:
        void RecalculateViewMatrix();

    private:
        glm::vec3 mRotation = { 0.0f, 0.0f, 0.0f };
    };
} // RealmFortress
