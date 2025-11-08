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
    class Camera;
    class GridRenderer;
    class ImGuiLayer;

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

        std::unique_ptr<Camera> m_Camera;
        std::unique_ptr<GridRenderer> m_Grid;
        std::unique_ptr<ImGuiLayer> m_ImGui;
    };
} // namespace RealmFortress
