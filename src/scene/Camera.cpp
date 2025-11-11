/**
 * @file Camera.cpp
 * @brief
 * @date 11/8/2025
 */

#include "Camera.h"
#include "events/Event.h"
#include "events/ApplicationEvent.h"
#include "events/MouseEvent.h"
#include "core/Input.h"
#include "core/KeyCodes.h"
#include "core/MouseCodes.h"
#include <glm/gtc/matrix_transform.hpp>

namespace RealmFortress
{
    Camera::Camera(float fov, float aspect, float near, float far)
        : m_FOV(fov), m_Aspect(aspect), m_Near(near), m_Far(far)
    {
        Recalculate();
    }

    void Camera::SetViewport(float width, float height)
    {
        m_Aspect = width / height;
        Recalculate();
    }

    void Camera::ProcessMouse(float dx, float dy)
    {
        constexpr float orbitSensitivity = 0.1f;
        m_Yaw += dx * orbitSensitivity;
        m_Pitch += -dy * orbitSensitivity;
        m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);
        Recalculate();
    }

    void Camera::ProcessScroll(float yoffset)
    {
        m_Distance -= yoffset * 0.8f;
        m_Distance = glm::clamp(m_Distance, 2.0f, 200.0f);
        Recalculate();
    }

    void Camera::ProcessPan(float dx, float dy)
    {
        constexpr float panBase = 0.005f;
        float speed = panBase * m_Distance;

        glm::vec3 forward = glm::normalize(m_Target - m_Position);
        glm::vec3 right   = glm::normalize(glm::cross(forward, m_Up));
        glm::vec3 up      = glm::normalize(glm::cross(right, forward));

        glm::vec3 delta = (-right) * dx + up * dy;

        m_Target += delta * speed;
        m_Position += delta * speed;
        Recalculate();
    }

    void Camera::Update(float dt)
    {
        (void)dt;
    }

    glm::mat4 Camera::GetViewMatrix() const
    {
        return m_View;
    }

    glm::mat4 Camera::GetProjectionMatrix() const
    {
        return m_Proj;
    }

    void Camera::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& e){ return OnMouseMoved(e); });
        dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& e){ return OnMouseScrolled(e); });
        dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& e){ return OnMouseButtonPressed(e); });
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e){ return OnWindowResize(e); });
    }

    void Camera::Recalculate()
    {
        float yaw = glm::radians(m_Yaw);
        float pitch = glm::radians(m_Pitch);
        glm::vec3 dir {
            cos(pitch) * cos(yaw),
            sin(pitch),
            cos(pitch) * sin(yaw)
        };
        m_Position = m_Target - dir * m_Distance;
        m_View = glm::lookAt(m_Position, m_Target, m_Up);
        m_Proj = glm::perspective(glm::radians(m_FOV), m_Aspect, m_Near, m_Far);
    }

    bool Camera::OnMouseMoved(MouseMovedEvent& event)
    {
        static bool isOrbiting = false, isPanning = false;
        static float lastX = event.GetX(), lastY = event.GetY();

        float dx = event.GetX() - lastX;
        float dy = event.GetY() - lastY;
        lastX = event.GetX(); lastY = event.GetY();

        if (Input::IsMouseButtonDown(MouseCode::Right))
        {
            ProcessMouse(dx, dy);
            return true;
        }
        if (Input::IsMouseButtonDown(MouseCode::Middle))
        {
            ProcessPan(dx, dy);
            return true;
        }
        return false;
    }

    bool Camera::OnMouseScrolled(MouseScrolledEvent& event)
    {
        ProcessScroll(event.GetYOffset());
        return true;
    }

    bool Camera::OnMouseButtonPressed(MouseButtonPressedEvent& event)
    {
        return false;
    }

    bool Camera::OnWindowResize(WindowResizeEvent& event)
    {
        SetViewport(event.GetWidth(), event.GetHeight());
        return false;
    }
} // namespace RealmFortress
