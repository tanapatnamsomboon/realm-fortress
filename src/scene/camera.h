/**
 * @file camera.h
 * @brief
 * @date 11/21/2025
 */

#pragma once

#include "core/base.h"
#include <glm/glm.hpp>

namespace RF
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

        const glm::mat4& GetProjection() const { return mProjection; }

    protected:
        glm::mat4 mProjection = glm::mat4(1.0f);
    };

    class OrthographicCamera final : public Camera
    {
    public:
        OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top);

        void SetProjection(f32 left, f32 right, f32 bottom, f32 top);

        const glm::vec3& GetPosition() const { return mPosition; }
        void SetPosition(const glm::vec3& position) { mPosition = position; RecalculateViewMatrix(); }

        f32 GetRotation() const { return mRotation; }
        void SetRotation(f32 rotation) { mRotation = rotation; RecalculateViewMatrix(); }

        const glm::mat4& GetViewMatrix() const { return mViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return mViewProjectionMatrix; }

    private:
        void RecalculateViewMatrix();

        glm::mat4 mViewMatrix;
        glm::mat4 mViewProjectionMatrix;

        glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f };
        f32 mRotation = 0.0f;
    };

    class PerspectiveCamera final : public Camera
    {
    public:
        PerspectiveCamera(f32 fov, f32 aspectRatio, f32 nearClip, f32 farClip);

        void SetProjection(f32 fov, f32 aspectRatio, f32 nearClip, f32 farClip);

        void SetPosition(const glm::vec3& position) { mPosition = position; RecalculateViewMatrix(); }
        void SetRotation(const glm::vec3& rotation) { mRotation = rotation; RecalculateViewMatrix(); }

        const glm::vec3& GetPosition() const { return mPosition; }
        const glm::vec3& GetRotation() const { return mRotation; }

        const glm::vec3& GetForward() const { return mForward; }
        const glm::vec3& GetUp() const { return mUp; }
        const glm::vec3& GetRight() const { return mRight; }

        f32 GetPitch() const { return mPitch; }
        f32 GetYaw() const { return mYaw; }

        void SetPitch(f32 pitch) { mPitch = pitch; UpdateCameraVectors(); RecalculateViewMatrix(); }
        void SetYaw(f32 yaw) { mYaw = yaw; UpdateCameraVectors(); RecalculateViewMatrix(); }

        const glm::mat4& GetViewMatrix() const { return mViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return mViewProjectionMatrix; }

    private:
        void UpdateCameraVectors();
        void RecalculateViewMatrix();

        glm::mat4 mViewMatrix;
        glm::mat4 mViewProjectionMatrix;

        glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f };
        glm::vec3 mRotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 mForward = { 0.0f, 0.0f, -1.0f };
        glm::vec3 mUp = { 0.0f, 1.0f, 0.0f };
        glm::vec3 mRight = { 1.0f, 0.0f, 0.0f };

        f32 mPitch = 0.0f;
        f32 mYaw = -90.0f;
    };
} // RF
