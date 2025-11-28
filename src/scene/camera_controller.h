/**
 * @file camera_controller.h
 * @brief
 * @date 11/27/2025
 */

#pragma once

#include "scene/camera.h"
#include "core/timestep.h"
#include "events/event.h"
#include "events/mouse_event.h"

namespace RF
{
    class CameraController
    {
    public:
        explicit CameraController(float aspectRatio);

        void OnUpdate(Timestep ts);
        void OnEvent(Event& event);

        PerspectiveCamera& GetCamera() { return mCamera; }
        const PerspectiveCamera& GetCamera() const { return mCamera; }

        void SetAspectRatio(float aspectRatio);

    private:
        bool OnMouseScrolled(MouseScrolledEvent& event);

    private:
        f32 mAspectRatio;
        f32 mFOV = 45.0f;
        f32 mNearClip = 0.1f;
        f32 mFarClip = 1000.0f;

        PerspectiveCamera mCamera;

        glm::vec3 mCameraPosition = { 0.0f, 10.0f, 10.0f };
        glm::vec3 mCameraRotation = { -45.0f, 0.0f, 0.0f };

        f32 mCameraTranslationSpeed = 5.0f;
        f32 mCameraRotationSpeed = 50.0f;

        glm::vec2 mLastMousePosition = { 0.0f, 0.0f };

    };
} // namespace RF
