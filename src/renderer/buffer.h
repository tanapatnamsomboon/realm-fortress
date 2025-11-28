/**
 * @file buffer.h
 * @brief
 * @date 11/21/2025
 */

#pragma once

#include "core/base.h"
#include "core/logger.h"

namespace RF
{
    enum class ShaderDataType
    {
        None = 0,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool
    };

    static u32 ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:  return 4;
            case ShaderDataType::Float2: return 4 * 2;
            case ShaderDataType::Float3: return 4 * 3;
            case ShaderDataType::Float4: return 4 * 4;
            case ShaderDataType::Mat3:   return 4 * 3 * 3;
            case ShaderDataType::Mat4:   return 4 * 4 * 4;
            case ShaderDataType::Int:    return 4;
            case ShaderDataType::Int2:   return 4 * 2;
            case ShaderDataType::Int3:   return 4 * 3;
            case ShaderDataType::Int4:   return 4 * 4;
            case ShaderDataType::Bool:   return 1;
            default: break;
        }
        RF_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    struct BufferElement
    {
        std::string mName;
        ShaderDataType mType;
        u32 mSize;
        usize mOffset;
        bool mNormalized;

        BufferElement() = default;

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : mName(name), mType(type), mSize(ShaderDataTypeSize(type)), mOffset(0), mNormalized(normalized)
        {
        }

        u32 GetComponentCount() const
        {
            switch (mType)
            {
                case ShaderDataType::Float:  return 1;
                case ShaderDataType::Float2: return 2;
                case ShaderDataType::Float3: return 3;
                case ShaderDataType::Float4: return 4;
                case ShaderDataType::Mat3:   return 3 * 3;
                case ShaderDataType::Mat4:   return 4 * 4;
                case ShaderDataType::Int:    return 1;
                case ShaderDataType::Int2:   return 2;
                case ShaderDataType::Int3:   return 3;
                case ShaderDataType::Int4:   return 4;
                case ShaderDataType::Bool:   return 1;
                default: break;
            }
            RF_CORE_ASSERT(false, "Unknown ShaderDataType!");
            return 0;
        }
    };

    class BufferLayout
    {
    public:
        BufferLayout() {}
        BufferLayout(const std::initializer_list<BufferElement>& elements)
            : mElements(elements)
        {
            CalculateOffsetsAndStride();
        }

        u32 GetStride() const { return mStride; }
        const std::vector<BufferElement>& GetElements() const { return mElements; }

        std::vector<BufferElement>::iterator begin() { return mElements.begin(); }
        std::vector<BufferElement>::iterator end() { return mElements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return mElements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return mElements.end(); }

    private:
        void CalculateOffsetsAndStride()
        {
            usize offset = 0;
            mStride = 0;
            for (auto& element : mElements)
            {
                element.mOffset = offset;
                offset += element.mSize;
                mStride += element.mSize;
            }
        }

    private:
        std::vector<BufferElement> mElements;
        u32 mStride = 0;
    };

    class VertexBuffer
    {
    public:
        VertexBuffer(u32 size);
        VertexBuffer(f32* vertices, u32 size);
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;

        void SetData(const void* data, u32 size);

        const BufferLayout& GetLayout() const { return mLayout; }
        void SetLayout(const BufferLayout& layout) { mLayout = layout; }

        static Ref<VertexBuffer> Create(u32 size);
        static Ref<VertexBuffer> Create(f32* vertices, u32 size);

    private:
        u32 mRendererID;
        BufferLayout mLayout;
    };

    class IndexBuffer
    {
    public:
        IndexBuffer(u32* indices, u32 count);
        ~IndexBuffer();

        void Bind() const;
        void Unbind() const;

        u32 GetCount() const { return mCount; }

        static Ref<IndexBuffer> Create(u32* indices, u32 count);

    private:
        u32 mRendererID;
        u32 mCount;
    };
} // namespace RF
