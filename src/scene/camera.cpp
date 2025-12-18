/**
 * @file camera.cpp
 * @brief
 * @date 11/21/2025
 */

#include "core/pch.h"
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace RealmFortress
{
    // Orthographic Camera
    OrthographicCamera::OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top)
        : Camera(glm::ortho(left, right, bottom, top, -1.0f, 1.0f))
    {
        mViewProjectionMatrix = mProjection * mViewMatrix;
    }

    void OrthographicCamera::SetProjection(f32 left, f32 right, f32 bottom, f32 top)
    {
        mProjection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        mViewProjectionMatrix = mProjection * mViewMatrix;
    }

    void OrthographicCamera::RecalculateViewMatrix()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), mPosition) *
            glm::rotate(glm::mat4(1.0f), glm::radians(mRotation), glm::vec3(0, 0, 1));

        mViewMatrix = glm::inverse(transform);
        mViewProjectionMatrix = mProjection * mViewMatrix;
    }

    // Perspective Camera
    PerspectiveCamera::PerspectiveCamera(f32 fov, f32 aspect_ratio, f32 near_clip, f32 far_clip)
    {
        SetProjection(fov, aspect_ratio, near_clip, far_clip);
    }

    void PerspectiveCamera::SetProjection(f32 fov, f32 aspect_ratio, f32 near_clip, f32 far_clip)
    {
        mProjection = glm::perspective(glm::radians(fov), aspect_ratio, near_clip, far_clip);
        mViewProjectionMatrix = mProjection * mViewMatrix;
    }

    glm::vec3 PerspectiveCamera::GetForward() const
    {
        glm::quat orientation = glm::quat(glm::vec3(
            glm::radians(mRotation.x),
            glm::radians(mRotation.y),
            glm::radians(mRotation.z)
        ));
        return glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 PerspectiveCamera::GetRight() const
    {
        glm::quat orientation = glm::quat(glm::vec3(
            glm::radians(mRotation.x),
            glm::radians(mRotation.y),
            glm::radians(mRotation.z)
        ));
        return glm::rotate(orientation, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 PerspectiveCamera::GetUp() const
    {
        glm::quat orientation = glm::quat(glm::vec3(
            glm::radians(mRotation.x),
            glm::radians(mRotation.y),
            glm::radians(mRotation.z)
        ));
        return glm::rotate(orientation, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void PerspectiveCamera::RecalculateViewMatrix()
    {
        glm::quat orientation = glm::quat(glm::vec3(
            glm::radians(mRotation.x),  // pitch
            glm::radians(mRotation.y),  // yaw
            glm::radians(mRotation.z)   // roll
        ));

        glm::mat4 rotation = glm::toMat4(orientation);
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), mPosition);

        mViewMatrix = glm::inverse(translation * rotation);
        mViewProjectionMatrix = mProjection * mViewMatrix;
    }

} // namespace RealmFortress
