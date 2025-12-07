/**
 * @file material.h
 * @brief
 * @date 11/25/2025
 */

#pragma once

#include "core/base.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include <glm/glm.hpp>

namespace RealmFortress
{
    class Material
    {
    public:
        Material(const Ref<Shader>& shader);

        void Bind();

        void SetTexture(const std::string& name, const Ref<Texture2D>& texture);
        void SetFloat(const std::string& name, float value);
        void SetFloat3(const std::string& name, const glm::vec3& value);
        void SetFloat4(const std::string& name, const glm::vec4& value);

    private:
        Ref<Shader> mShader;
        std::unordered_map<std::string, Ref<Texture2D>> mTextures;
        std::unordered_map<std::string, float> mFloats;
        std::unordered_map<std::string, glm::vec3> mFloat3s;
        std::unordered_map<std::string, glm::vec4> mFloat4s;
    };
} // namespace RealmFortress
