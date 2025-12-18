/**
 * @file buffer.cpp
 * @brief
 * @date 11/21/2025
 */

#include "buffer.h"
#include "core/logger.h"
#include <glad/gl.h>

namespace RealmFortress
{
    VertexBuffer::VertexBuffer(u32 size)
    {
        glCreateBuffers(1, &mRendererID);
        glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    }

    VertexBuffer::VertexBuffer(f32* vertices, u32 size)
    {
        glCreateBuffers(1, &mRendererID);
        glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }

    VertexBuffer::~VertexBuffer()
    {
        glDeleteBuffers(1, &mRendererID);
    }

    void VertexBuffer::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
    }

    void VertexBuffer::Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::SetData(const void* data, u32 size)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }

    Ref<VertexBuffer> VertexBuffer::Create(u32 size)
    {
        return CreateRef<VertexBuffer>(size);
    }

    Ref<VertexBuffer> VertexBuffer::Create(f32* vertices, u32 size)
    {
        return CreateRef<VertexBuffer>(vertices, size);
    }

    IndexBuffer::IndexBuffer(u32* indices, u32 count)
        : mCount(count)
    {
        glCreateBuffers(1, &mRendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), indices, GL_STATIC_DRAW);
    }

    IndexBuffer::~IndexBuffer()
    {
        glDeleteBuffers(1, &mRendererID);
    }

    void IndexBuffer::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererID);
    }

    void IndexBuffer::Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    Ref<IndexBuffer> IndexBuffer::Create(u32* indices, u32 count)
    {
        return CreateRef<IndexBuffer>(indices, count);
    }
} // namespace RealmFortress
