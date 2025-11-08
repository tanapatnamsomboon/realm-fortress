/**
 * @file Shader.cpp
 * @brief
 * @date 11/8/2025
 */

#include "Shader.h"
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

namespace Utils
{
    std::string ReadFile(const std::string& path)
    {
        std::ifstream ifs(path);
        if (!ifs) throw std::runtime_error("Failed to open: " + path);
        std::stringstream ss;
        ss << ifs.rdbuf();
        return ss.str();
    }
} // namespace Utils

namespace RealmFortress
{
    Shader::Shader(const std::string& vertPath, const std::string& fragPath)
    {
        LoadFromFiles(vertPath, fragPath);
    }

    Shader::~Shader()
    {
        if (m_ID)
            glDeleteProgram(m_ID);
    }

    bool Shader::LoadFromFiles(const std::string& vertPath, const std::string& fragPath)
    {
        const auto vsSrc = Utils::ReadFile(vertPath);
        const auto fsSrc = Utils::ReadFile(fragPath);

        unsigned int vs = Compile(GL_VERTEX_SHADER, vsSrc);
        unsigned int fs = Compile(GL_FRAGMENT_SHADER, fsSrc);
        m_ID = Link(vs, fs);
        glDeleteProgram(vs);
        glDeleteProgram(fs);
        return m_ID != 0;
    }

    void Shader::Use() const
    {
        glUseProgram(m_ID);
    }

    void Shader::SetInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
    }

    void Shader::SetFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(value));
    }

    unsigned int Shader::Compile(unsigned int type, const std::string& src) const
    {
        unsigned int id = glCreateShader(type);
        const char* source = src.c_str();
        glShaderSource(id, 1, &source, nullptr);
        glCompileShader(id);

        int ok;
        glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            char log[1024];
            glGetShaderInfoLog(id, 1024, nullptr, log);
            std::cerr << "[Shader] Compile error: " << log << std::endl;
            glDeleteShader(id);
            throw std::runtime_error("Shader compile failed");
        }
        return id;
    }

    unsigned int Shader::Link(unsigned int vs, unsigned int fs) const
    {
        unsigned int prog = glCreateProgram();
        glAttachShader(prog, vs);
        glAttachShader(prog, fs);
        glLinkProgram(prog);

        int ok;
        glGetProgramiv(prog, GL_LINK_STATUS, &ok);
        if (!ok)
        {
            char log[1024];
            glGetProgramInfoLog(prog, 1024, nullptr, log);
            std::cerr << "[Shader] Link error: " << log << std::endl;
            glDeleteProgram(prog);
            throw std::runtime_error("Shader link failed");
        }
        return prog;
    }
} // namespace RealmFortress
