/**
 * @file camera_controller.cpp
 * @brief
 * @date 11/27/2025
 */

#include "core/pch.h"
#include "camera_controller.h"
#include "core/input.h"
#include "core/key_codes.h"
#include "core/mouse_codes.h"
#include "events/mouse_event.h"

namespace RealmFortress
{
    CameraController::CameraController(float aspectRatio)
        : mAspectRatio(aspectRatio), mCamera(mFOV, aspectRatio, mNearClip, mFarClip)
    {
        mCamera.SetPosition(mCameraPosition);
        mCamera.SetRotation(mCameraRotation);
    }

    void CameraController::OnUpdate(Timestep ts)
    {
        glm::vec3 forward = mCamera.GetForward();
        forward.y = 0;
        forward = glm::normalize(forward);

        glm::vec3 right = mCamera.GetRight();
        right.y = 0;
        right = glm::normalize(right);

        f32 speed = mCameraTranslationSpeed * static_cast<f32>(ts);

        mZoomLevel += (mTargetZoomLevel - mZoomLevel) * 8.0f * ts;

        speed *= mZoomLevel;

        if (Input::IsKeyPressed(Key::W)) mCameraPosition += forward * speed;
        if (Input::IsKeyPressed(Key::S)) mCameraPosition -= forward * speed;
        if (Input::IsKeyPressed(Key::A)) mCameraPosition -= right * speed;
        if (Input::IsKeyPressed(Key::D)) mCameraPosition += right * speed;

        if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
        {
            glm::vec2 mouse_pos = Input::GetMousePosition();
            glm::vec2 delta = mouse_pos - mLastMousePosition;

            mYaw -= delta.x * mCameraRotationSpeed * static_cast<float>(ts);
        }
        mLastMousePosition = Input::GetMousePosition();

        mCameraRotation = glm::vec3(-60.0f, mYaw, 0.0f);

        mCameraPosition.y = mZoomLevel * 10.0f;

        mCamera.SetPosition(mCameraPosition);
        mCamera.SetRotation(mCameraRotation);
    }

    void CameraController::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseScrolledEvent>(RF_BIND_EVENT_FN(CameraController::OnMouseScrolled));
    }

    void CameraController::SetAspectRatio(float aspectRatio)
    {
        mAspectRatio = aspectRatio;
        mCamera.SetProjection(mFOV, mAspectRatio, mNearClip, mFarClip);
    }

    bool CameraController::OnMouseScrolled(MouseScrolledEvent& event)
    {
        mTargetZoomLevel -= event.GetYOffset() * 0.25f;
        mTargetZoomLevel = std::clamp(mTargetZoomLevel, 0.5f, 5.0f);
        return false;
    }
} // namespace RealmFortress
