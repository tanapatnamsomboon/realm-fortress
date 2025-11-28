/**
 * @file renderer_api.h
 * @brief
 * @date 11/21/2025
 */

#pragma once

#include "core/base.h"
#include <glm/glm.hpp>

namespace RF
{
    class VertexArray;

    class RendererAPI
    {
    public:
        void Init();
        void SetViewport(u32 x, u32 y, u32 width, u32 height);
        void SetClearColor(const glm::vec4& color);
        void Clear();

        void DrawIndexed(const Ref<VertexArray>& vertexArray, u32 indexCount = 0);
        void DrawArrays(const Ref<VertexArray>& vertexArray, u32 vertexCount);
    };
} // namespace RF
