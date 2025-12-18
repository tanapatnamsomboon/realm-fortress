/**
 * @file frame_buffer.h
 * @brief
 * @date 12/17/2025
 */

#pragma once

#include "core/base.h"
#include <vector>

namespace RealmFortress
{
    enum class FramebufferTextureFormat
    {
        None = 0,

        // Color
        RGBA8,
        RED_INTEGER,

        // Depth / Stencil
        DEPTH24STENCIL8,

        // Defaults
        Depth = DEPTH24STENCIL8,
    };

    struct FramebufferTextureSpecification
    {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(FramebufferTextureFormat format)
            : TextureFormat(format)
        {
        }

        FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
    };

    struct FramebufferAttachmentSpecification
    {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
            : Attachments(attachments)
        {
        }

        std::vector<FramebufferTextureSpecification> Attachments;
    };

    struct FramebufferSpecification
    {
        u32 Width{ 0 }, Height{ 0 };
        FramebufferAttachmentSpecification Attachments;
        u32 Samples = 1;

        bool SwapChainTarget = false;
    };

    class Framebuffer
    {
    public:
        Framebuffer(const FramebufferSpecification& spec);
        ~Framebuffer();

        void Invalidate();

        void Bind();
        void Unbind();

        void Resize(u32 width, u32 height);
        i32 ReadPixel(u32 attachment_index, i32 x, i32 y);

        void ClearAttachment(i32 attachment_index, i32 value);

        u32 GetColorAttachmentRendererID(u32 index = 0) const { RF_CORE_ASSERT(index < mColorAttachments.size()); return mColorAttachments[index]; }

        const FramebufferSpecification& GetSpecification() const { return mSpecification; }

        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);

    private:
        uint32_t mRendererID = 0;
        FramebufferSpecification mSpecification;

        std::vector<FramebufferTextureSpecification> mColorAttachmentSpecifications;
        FramebufferTextureSpecification mDepthAttachmentSpecification = FramebufferTextureFormat::None;

        std::vector<uint32_t> mColorAttachments;
        uint32_t mDepthAttachment = 0;
    };
} // namespace RealmFortress
