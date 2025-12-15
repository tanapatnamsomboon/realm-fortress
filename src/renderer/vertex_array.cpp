/**
 * @file vertex_array.cpp
 * @brief
 * @date 11/21/2025
 */

#include "vertex_array.h"
#include "core/logger.h"
#include <glad/gl.h>

namespace RealmFortress
{
    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:   return GL_FLOAT;
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:   return GL_INT;
            case ShaderDataType::Bool:   return GL_BOOL;
            default: break;
        }
        RF_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    VertexArray::VertexArray()
    {
        glCreateVertexArrays(1, &mRendererID);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &mRendererID);
    }

    void VertexArray::Bind() const
    {
        glBindVertexArray(mRendererID);
    }

    void VertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertex_buffer)
    {
        RF_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

        glBindVertexArray(mRendererID);
        vertex_buffer->Bind();

        const auto& layout = vertex_buffer->GetLayout();
        for (const auto& element : layout)
        {
            glEnableVertexAttribArray(mVertexBufferIndex);
            glVertexAttribPointer(mVertexBufferIndex,
                element.GetComponentCount(),
                ShaderDataTypeToOpenGLBaseType(element.mType),
                element.mNormalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)element.mOffset);
            mVertexBufferIndex++;
        }

        mVertexBuffers.push_back(vertex_buffer);
    }

    void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& index_buffer)
    {
        glBindVertexArray(mRendererID);
        index_buffer->Bind();

        mIndexBuffer = index_buffer;
    }

    Ref<VertexArray> VertexArray::Create()
    {
        return CreateRef<VertexArray>();
    }
} // namespace RealmFortress
