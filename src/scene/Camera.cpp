/**
 * @file Camera.cpp
 * @brief
 * @date 11/8/2025
 */

#include "Camera.h"
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
        m_Yaw += dx * 0.1f;
        m_Pitch += dy * 0.1f;
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
        float speed = 0.02f * m_Distance;
        glm::vec3 right = glm::normalize(glm::cross(m_Target - m_Position, m_Up));
        glm::vec3 up = glm::normalize(m_Up);
        m_Target += (-right * dx + up * dy) * speed;
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
} // namespace RealmFortress
