/**
 * @file material.cpp
 * @brief
 * @date 11/25/2025
 */

#include "material.h"

namespace RealmFortress
{
    Material::Material(const Ref<Shader>& shader)
        : mShader(shader)
    {
    }

    void Material::Bind()
    {
        mShader->Bind();

        u32 slot = 0;
        for (auto& [name, texture] : mTextures)
        {
            texture->Bind(slot);
            mShader->SetInt(name, slot);
            slot++;
        }

        for (auto& [name, value] : mFloats)
            mShader->SetInt(name, value);

        for (auto& [name, value] : mFloat3s)
            mShader->SetFloat3(name, value);

        for (auto& [name, value] : mFloat4s)
            mShader->SetFloat4(name, value);
    }

    void Material::SetTexture(const std::string& name, const Ref<Texture2D>& texture)
    {
        mTextures[name] = texture;
    }

    void Material::SetFloat(const std::string& name, float value)
    {
        mFloats[name] = value;
    }

    void Material::SetFloat3(const std::string& name, const glm::vec3& value)
    {
        mFloat3s[name] = value;
    }

    void Material::SetFloat4(const std::string& name, const glm::vec4& value)
    {
        mFloat4s[name] = value;
    }
} // namespace RealmFortress
