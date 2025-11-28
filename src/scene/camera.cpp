/**
 * @file camera.cpp
 * @brief
 * @date 11/21/2025
 */

#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace RF
{
    // Orthographic Camera
    OrthographicCamera::OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top)
        : Camera(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), mViewMatrix(1.0f)
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
    PerspectiveCamera::PerspectiveCamera(f32 fov, f32 aspectRatio, f32 nearClip, f32 farClip)
        : Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip)), mViewMatrix(1.0f)
    {
        UpdateCameraVectors();
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::SetProjection(f32 fov, f32 aspectRatio, f32 nearClip, f32 farClip)
    {
        mProjection = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
        mViewProjectionMatrix = mProjection * mViewMatrix;
    }

    void PerspectiveCamera::UpdateCameraVectors()
    {
        glm::vec3 forward;
        forward.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        forward.y = sin(glm::radians(mPitch));
        forward.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        mForward = glm::normalize(forward);

        mRight = glm::normalize(glm::cross(mForward, glm::vec3(0.0f, 1.0f, 0.0f)));
        mUp = glm::normalize(glm::cross(mRight, mForward));
    }

    void PerspectiveCamera::RecalculateViewMatrix()
    {
        mViewMatrix = glm::lookAt(mPosition, mPosition + mForward, mUp);
        mViewProjectionMatrix = mProjection * mViewMatrix;
    }
} // RF
