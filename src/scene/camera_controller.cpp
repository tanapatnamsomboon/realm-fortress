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

namespace RF
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
        glm::vec3 right = mCamera.GetRight();

        // forward/backward
        if (Input::IsKeyPressed(Key::W))
            mCameraPosition += forward * mCameraTranslationSpeed * static_cast<float>(ts);
        if (Input::IsKeyPressed(Key::S))
            mCameraPosition -= forward * mCameraTranslationSpeed * static_cast<float>(ts);

        // left/right
        if (Input::IsKeyPressed(Key::A))
            mCameraPosition -= right * mCameraTranslationSpeed * static_cast<float>(ts);
        if (Input::IsKeyPressed(Key::D))
            mCameraPosition += right * mCameraTranslationSpeed * static_cast<float>(ts);

        // up/down
        if (Input::IsKeyPressed(Key::Q))
            mCameraPosition.y += mCameraTranslationSpeed * static_cast<float>(ts);
        if (Input::IsKeyPressed(Key::E))
            mCameraPosition.y -= mCameraTranslationSpeed * static_cast<float>(ts);

        // camera rotation with mouse (right-click)
        if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
        {
            glm::vec2 mousePos = Input::GetMousePosition();
            glm::vec2 delta = mousePos - mLastMousePosition;

            mCameraRotation.y += delta.x * mCameraRotationSpeed * static_cast<float>(ts);
            mCameraRotation.x -= delta.y * mCameraRotationSpeed * static_cast<float>(ts);

            if (mCameraRotation.x > 89.0f)
                mCameraRotation.x = 89.0f;
            if (mCameraRotation.x < -89.0f)
                mCameraRotation.x = -89.0f;
        }

        mLastMousePosition = Input::GetMousePosition();

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
        mCameraTranslationSpeed += event.GetYOffset() * 0.5f;
        mCameraTranslationSpeed = std::max(mCameraTranslationSpeed, 0.25f);
        return false;
    }
} // namespace RF
