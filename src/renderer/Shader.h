/**
 * @file Shader.h
 * @brief
 * @date 11/8/2025
 */

#pragma once

#include <glm/glm.hpp>
#include <string>

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
        Shader() = default;
        Shader(const std::string& vertPath, const std::string& fragPath);
        ~Shader();

        bool LoadFromFiles(const std::string& vertPath, const std::string& fragPath);
        void Use() const;

        void SetInt(const std::string& name, int value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetMat4(const std::string& name, const glm::mat4& value) const;
        void SetVec3(const std::string& name, const glm::vec3& value) const;

        unsigned int ID() const { return m_ID; }

    private:
        unsigned int Compile(unsigned int type, const std::string& src) const;
        unsigned int Link(unsigned int vs, unsigned int fs) const;

    private:
        unsigned int m_ID = 0;
    };
} // namespace RealmFortress
