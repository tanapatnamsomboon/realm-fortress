/**
 * @file renderer.h
 * @brief
 * @date 11/9/2025
 */

#pragma once

#include "core/base.h"
#include "renderer/vertex_array.h"
#include "renderer/shader.h"
#include "scene/camera.h"
#include <glm/glm.hpp>

namespace RealmFortress
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

        static void BeginFrame();
        static void EndFrame();

        static void BeginScene(const Camera& camera);
        static void EndScene();

        static void DrawMesh(const Ref<Shader>&      shader,
                             const Ref<VertexArray>& vertex_array,
                             const glm::mat4&        transform = glm::mat4(1.0f));

        static void DrawIndexed(const Ref<VertexArray>& vertex_array,
                                u32                     index_count = 0);

        static void DrawArrays(const Ref<VertexArray>& vertex_array,
                               u32                     vertex_count);

        static void SetClearColor(const glm::vec4& color);
        static void SetViewport(u32 x, u32 y, u32 width, u32 height);
        static void SetBlendMode(bool enabled);
        static void SetDepthTest(bool enabled);
        static void SetCullFace(bool enabled);
        static void SetWireframe(bool enabled);
        static void SetSceneViewProjection(const glm::mat4& view_projection);

        static void Clear();

        struct Statistics
        {
            u32 DrawCalls = 0;
            u32 VertexCount = 0;
            u32 IndexCount = 0;
            u32 TriangleCount = 0;

            void Reset()
            {
                DrawCalls = 0;
                VertexCount = 0;
                IndexCount = 0;
                TriangleCount = 0;
            }
        };

        static Statistics& GetStats() { return sStats; }
        static void ResetStats() { sStats.Reset(); }

        static const glm::mat4& GetViewProjectionMatrix()
        {
            return sSceneData.ViewProjectionMatrix;
        }

    private:
        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix{ 1.0f };
        };

        static SceneData sSceneData;
        static Statistics sStats;
    };
} // namespace RealmFortress
