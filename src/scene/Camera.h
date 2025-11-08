/**
 * @file Camera.h
 * @brief
 * @date 11/8/2025
 */

#pragma once

#include <glm/glm.hpp>

namespace RealmFortress
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
        /**
         * @brief Constructs a Camera object with the specified parameters.
         *
         * Initializes a camera with the given field of view (FOV), aspect ratio,
         * and near and far clipping planes.
         *
         * @param fov The field of view in degrees.
         * @param aspect The aspect ratio of the camera's viewport.
         * @param near The near-clipping plane distance.
         * @param far The far-clipping plane distance.
         */
        Camera(float fov, float aspect, float near, float far);

        void SetViewport(float width, float height);
        void ProcessMouse(float dx, float dy);
        void ProcessScroll(float yoffset);
        void ProcessPan(float dx, float dy);
        void Update(float dt);

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;

        glm::vec3 Position() const { return m_Position; }

    private:
        void Recalculate();

    private:
        float m_FOV{ 45.0f };
        float m_Aspect{ 1.778f };
        float m_Near{ 0.1f };
        float m_Far{ 500.0f };

        float m_Distance{ 10.0f };
        float m_Yaw{ -90.0f };
        float m_Pitch{ -30.0f };

        glm::vec3 m_Position{ 0.0f };
        glm::vec3 m_Target{ 0.0f };
        glm::vec3 m_Up{ 0.0f, 1.0f, 0.0f };

        glm::mat4 m_View{ 1.0f };
        glm::mat4 m_Proj{ 1.0f };
    };
} // namespace RealmFortress
