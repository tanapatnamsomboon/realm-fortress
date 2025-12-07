/**
 * @file shader.h
 * @brief
 * @date 11/8/2025
 */

#pragma once

#include "core/base.h"
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace RealmFortress
{
    /**
     * @class Shader
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Shader
    {
    public:
        Shader(const std::string& name, const std::string& vertex, const std::string& fragment);
        Shader(const std::string& filepath);
        ~Shader();

        void Bind() const;
        void Unbind() const;

        void SetInt(const std::string& name, i32 value) const;
        void SetIntArray(const std::string& name, i32* values, u32 count) const;
        void SetFloat(const std::string& name, f32 value) const;
        void SetFloat2(const std::string& name, const glm::vec2& value) const;
        void SetFloat3(const std::string& name, const glm::vec3& value) const;
        void SetFloat4(const std::string& name, const glm::vec4& value) const;
        void SetMat3(const std::string& name, const glm::mat3& value) const;
        void SetMat4(const std::string& name, const glm::mat4& value) const;

        const std::string& GetName() const { return mName; }

        static Ref<Shader> Create(const std::string& name, const std::string& vertex, const std::string& fragment);
        static Ref<Shader> Create(const std::string& filepath);

    private:
        void Compile(const std::unordered_map<u32, std::string>& shaderSources);
        i32 GetUniformLocation(const std::string& name) const;

    private:
        u32 mRendererID;
        std::string mName;
        mutable std::unordered_map<std::string, i32> mUniformLocationCache;
    };

    class ShaderLibrary
    {
    public:
        void Add(const std::string& name, const Ref<Shader>& shader);
        void Add(const Ref<Shader>& shader);
        Ref<Shader> Load(const std::string& filepath);
        Ref<Shader> Load(const std::string& name, const std::string& filepath);

        Ref<Shader> Get(const std::string& name);

        bool Exist(const std::string& name) const;

    private:
        std::unordered_map<std::string, Ref<Shader>> mShaders;
    };
} // namespace RealmFortress
