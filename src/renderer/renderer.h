/**
 * @file renderer.h
 * @brief
 * @date 11/9/2025
 */

#pragma once

#include "render_command.h"
#include "../scene/camera.h"
#include "shader.h"

namespace RF
{
    /**
     * @class Renderer
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void OnWindowResize(u32 width, u32 height);

        static void BeginScene(const Camera& camera);
        static void EndScene();

        static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray,
                           const glm::mat4& transform = glm::mat4(1.0f));
    private:
        struct SceneData
        {
            glm::mat4 mViewProjectionMatrix;
        };

        static Scope<SceneData> sSceneData;
    };
} // namespace RF
