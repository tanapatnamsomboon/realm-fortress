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
#include "events/application_event.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

#ifndef M_PI
#   define M_PI 3.14159265358979323846f
#endif

namespace RealmFortress
{
    CameraController::CameraController(f32 aspect_ratio)
        : mAspectRatio(aspect_ratio), mCamera(mFOV, aspect_ratio, 0.1f, 1000.0f)
    {
        UpdateCameraView();
    }

    void CameraController::OnUpdate(Timestep ts)
    {
        mDistance += (mTargetDistance - mDistance) * (1.0f - std::exp(-mZoomSmoothness * ts));

        if (Input::IsKeyPressed(Key::Q))
            mYaw += mRotationSpeed * ts;
        if (Input::IsKeyPressed(Key::E))
            mYaw -= mRotationSpeed * ts;

        if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
        {
            glm::vec2 mouse_pos = Input::GetMousePosition();
            glm::vec2 delta = mouse_pos - mLastMousePosition;

            mYaw -= delta.x * 0.3f;
            mPitch += delta.y * 0.3f;
            mPitch = std::clamp(mPitch, MIN_PITCH, MAX_PITCH);
        }
        mLastMousePosition = Input::GetMousePosition();

        f32 yaw_rad = glm::radians(mYaw);

        glm::vec3 forward_dir = glm::normalize(glm::vec3(
            -sin(yaw_rad),
            0.0f,
            -cos(yaw_rad)
        ));

        glm::vec3 right_dir = glm::cross(forward_dir, glm::vec3(0.0f, 1.0f, 0.0f));

        f32 speed = mMoveSpeed * (mDistance / 10.0f) * ts;

        if (Input::IsKeyPressed(Key::W)) mFocalPoint += forward_dir * speed;
        if (Input::IsKeyPressed(Key::S)) mFocalPoint -= forward_dir * speed;
        if (Input::IsKeyPressed(Key::A)) mFocalPoint -= right_dir * speed;
        if (Input::IsKeyPressed(Key::D)) mFocalPoint += right_dir * speed;

        UpdateCameraView();
    }

    void CameraController::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseScrolledEvent>(RF_BIND_EVENT_FN(CameraController::OnMouseScrolled));
    }

    void CameraController::SetAspectRatio(f32 aspect_ratio)
    {
        mAspectRatio = aspect_ratio;
        mCamera.SetProjection(mFOV, mAspectRatio, 0.1f, 1000.0f);
    }

    bool CameraController::OnMouseScrolled(MouseScrolledEvent& event)
    {
        mTargetDistance -= event.GetYOffset() * mZoomSpeed;
        mTargetDistance = std::clamp(mTargetDistance, MIN_DISTANCE, MAX_DISTANCE);
        return false;
    }

    bool CameraController::OnWindowResize(WindowResizeEvent& event)
    {
        return false;
    }

    void CameraController::UpdateCameraView()
    {
        f32 yaw_rad = glm::radians(mYaw);
        f32 pitch_rad = glm::radians(mPitch);

        f32 h_dist = mDistance * cos(pitch_rad);
        f32 v_dist = mDistance * sin(pitch_rad);

        f32 offset_x = h_dist * sin(yaw_rad);
        f32 offset_z = h_dist * cos(yaw_rad);

        glm::vec3 camera_pos;
        camera_pos.x = mFocalPoint.x + offset_x;
        camera_pos.y = mFocalPoint.y + v_dist;
        camera_pos.z = mFocalPoint.z + offset_z;

        mCamera.SetPosition(camera_pos);

        mCamera.SetRotation(glm::vec3(-mPitch, mYaw, 0.0f));
    }
} // namespace RealmFortress
