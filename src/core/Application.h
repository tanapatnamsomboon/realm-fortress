/**
 * @file Application.h
 * @brief
 * @date 11/8/2025
 */

#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace RealmFortress
{
    class Window;
    class Shader;

    /**
     * @class Application
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Application
    {
    public:
        Application();
        ~Application();

        void Run();

    private:
        void InitTriangle();
        void DrawTriangle();

    private:
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<Shader> m_Shader;

        unsigned int m_VAO{ 0 }, m_VBO{ 0 };
        glm::vec3 m_ClearColor{ 0.06f, 0.08f, 0.10f };
    };
} // namespace RealmFortress
