/**
 * @file texture.cpp
 * @brief
 * @date 11/21/2025
 */

#include "texture.h"
#include "core/logger.h"
#include <glad/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace RF
{
    Texture2D::Texture2D(u32 width, u32 height)
        : mWidth(width), mHeight(height)
    {
        mInternalFormat = GL_RGBA8;
        mDataFormat = GL_RGBA;

        glCreateTextures(GL_TEXTURE_2D, 1, &mRendererID);
        glTextureStorage2D(mRendererID, 1, mInternalFormat, mWidth, mHeight);

        glTextureParameteri(mRendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(mRendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    Texture2D::Texture2D(const std::string& path)
        : mPath(path)
    {
        i32 width, height, channels;
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (data)
        {
            mWidth = width;
            mHeight = height;

            GLenum internalFormat = 0, dataFormat = 0;
            if (channels == 4)
            {
                internalFormat = GL_RGBA8;
                dataFormat = GL_RGBA;
            }
            else if (channels == 3)
            {
                internalFormat = GL_RGB8;
                dataFormat = GL_RGB;
            }

            mInternalFormat = internalFormat;
            mDataFormat = dataFormat;

            RF_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

            glCreateTextures(GL_TEXTURE_2D, 1, &mRendererID);
            glTextureStorage2D(mRendererID, 1, internalFormat, mWidth, mHeight);

            glTextureParameteri(mRendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(mRendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTextureSubImage2D(mRendererID, 0, 0, 0, mWidth, mHeight, dataFormat, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }
        else
        {
            RF_CORE_ERROR("Failed to load texture: {}", path);
        }
    }

    Texture2D::~Texture2D()
    {
        glDeleteTextures(1, &mRendererID);
    }

    void Texture2D::SetData(void* data, u32 size)
    {
        u32 bpp = mDataFormat == GL_RGBA ? 4 : 3;
        RF_CORE_ASSERT(size == mWidth * mHeight * bpp, "Data must be entire texture!");
        glTextureSubImage2D(mRendererID, 0, 0, 0, mWidth, mHeight, mDataFormat, GL_UNSIGNED_BYTE, data);
    }

    void Texture2D::Bind(u32 slot) const
    {
        glBindTextureUnit(slot, mRendererID);
    }

    Ref<Texture2D> Texture2D::Create(u32 width, u32 height)
    {
        return CreateRef<Texture2D>(width, height);
    }

    Ref<Texture2D> Texture2D::Create(const std::string& path)
    {
        return CreateRef<Texture2D>(path);
    }
} // namespace RF
