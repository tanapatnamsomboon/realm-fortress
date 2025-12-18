/**
 * @file camera_controller.h
 * @brief
 * @date 11/27/2025
 */

#pragma once

#include "scene/camera.h"
#include "core/timestep.h"
#include "events/event.h"

namespace RealmFortress
{
    class CameraController
    {
    public:
        CameraController(f32 aspect_ratio);

        void OnUpdate(Timestep ts);
        void OnEvent(Event& event);

        PerspectiveCamera& GetCamera() { return mCamera; }
        const PerspectiveCamera& GetCamera() const { return mCamera; }
        const glm::vec3& GetFocalPoint() const { return mFocalPoint; }

        void SetAspectRatio(f32 aspect_ratio);
        void SetFocalPoint(const glm::vec3& point) { mFocalPoint = point; }

    private:
        bool OnMouseScrolled(class MouseScrolledEvent& event);
        bool OnWindowResize(class WindowResizeEvent& event);

        void UpdateCameraView();

    private:
        f32 mAspectRatio;
        f32 mFOV{ 60.0f };
        PerspectiveCamera mCamera;

        glm::vec3 mFocalPoint{ 0.0f, 0.0f, 0.0f };
        f32 mDistance{ 10.0f };
        f32 mTargetDistance{ 10.0f };
        f32 mZoomSmoothness{ 10.0f };
        f32 mYaw{ 0.0f };
        f32 mPitch{ 45.0f };

        f32 mMoveSpeed{ 10.0f };
        f32 mRotationSpeed{ 100.0f };
        f32 mZoomSpeed{ 2.0f };

        glm::vec2 mLastMousePosition{ 0.0f, 0.0f };

        const f32 MIN_DISTANCE{ 5.0f };
        const f32 MAX_DISTANCE{ 50.0f };

        const f32 MIN_PITCH{ 10.0f };
        const f32 MAX_PITCH{ 85.0f };
    };
} // namespace RealmFortress
