/**
 * @file texture.h
 * @brief
 * @date 11/21/2025
 */

#pragma once

#include "core/base.h"
#include <string>
#include <glad/gl.h>

namespace RealmFortress
{
    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual u32 GetWidth() const = 0;
        virtual u32 GetHeight() const = 0;
        virtual u32 GetRendererID() const = 0;

        virtual void SetData(void* data, u32 size) = 0;

        virtual void Bind(u32 slot = 0) const = 0;

        virtual bool operator==(const Texture& other) const = 0;
    };

    class Texture2D final : public Texture
    {
    public:
        Texture2D(u32 width, u32 height);
        Texture2D(const std::string& path);
        ~Texture2D() override;

        u32 GetWidth() const override { return mWidth; }
        u32 GetHeight() const override { return mHeight; }
        u32 GetRendererID() const override { return mRendererID; }

        void SetData(void* data, u32 size) override;

        void Bind(u32 slot) const override;

        bool operator==(const Texture& other) const override
        {
            return mRendererID == other.GetRendererID();
        }

        static Ref<Texture2D> Create(u32 width, u32 height);
        static Ref<Texture2D> Create(const std::string& path);

    private:
        std::string mPath;
        u32 mWidth, mHeight;
        u32 mRendererID;
        GLenum mInternalFormat, mDataFormat;
    };
} // namespace RealmFortress
