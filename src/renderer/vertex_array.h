/**
 * @file vertex_array.h
 * @brief
 * @date 11/21/2025
 */

#pragma once

#include "core/base.h"
#include "renderer/buffer.h"
#include <vector>

namespace RealmFortress
{
    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        void Bind() const;
        void Unbind() const;

        void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
        void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

        const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return mVertexBuffers; }
        const Ref<IndexBuffer>& GetIndexBuffer() const { return mIndexBuffer; }

        static Ref<VertexArray> Create();

    private:
        u32 mRendererID;
        u32 mVertexBufferIndex = 0;
        std::vector<Ref<VertexBuffer>> mVertexBuffers;
        Ref<IndexBuffer> mIndexBuffer;
    };
} // namespace RealmFortress
