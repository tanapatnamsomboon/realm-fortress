/**
 * @file render_command.h
 * @brief
 * @date 11/21/2025
 */

#pragma once

#include "renderer/renderer_api.h"

namespace RF
{
    class RenderCommand
    {
    public:
        static void Init()
        {
            sRendererAPI->Init();
        }

        static void SetViewport(u32 x, u32 y, u32 width, u32 height)
        {
            sRendererAPI->SetViewport(x, y, width, height);
        }

        static void SetClearColor(const glm::vec4& color)
        {
            sRendererAPI->SetClearColor(color);
        }

        static void Clear()
        {
            sRendererAPI->Clear();
        }

        static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
        {
            sRendererAPI->DrawIndexed(vertexArray, indexCount);
        }

        static void DrawArrays(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
        {
            sRendererAPI->DrawArrays(vertexArray, vertexCount);
        }

    private:
        static Scope<RendererAPI> sRendererAPI;
    };
} // namespace RF
