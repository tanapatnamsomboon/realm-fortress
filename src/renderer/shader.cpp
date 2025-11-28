/**
 * @file shader.cpp
 * @brief
 * @date 11/8/2025
 */

#include "shader.h"
#include "core/logger.h"
#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>

namespace RF
{
    static GLenum ShaderTypeFromString(const std::string& type)
    {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;

        RF_CORE_ASSERT(false, "Unknown shader type!");
        return 0;
    }

    Shader::Shader(const std::string& name, const std::string& vertex, const std::string& fragment)
        : mName(name)
    {
        std::unordered_map<u32, std::string> sources;
        sources[GL_VERTEX_SHADER] = vertex;
        sources[GL_FRAGMENT_SHADER] = fragment;
        Compile(sources);
    }

    Shader::Shader(const std::string& filepath)
    {
        std::string source;
        std::ifstream file(filepath);
        if (file)
        {
            std::stringstream ss;
            ss << file.rdbuf();
            source = ss.str();
        }
        else
        {
            RF_CORE_ERROR("Failed to read shader file: {}", filepath);
            return;
        }

        std::unordered_map<u32, std::string> sources;
        const char* token = "#type";
        usize length = strlen(token);
        usize pos = source.find(token, 0);

        while (pos != std::string::npos)
        {
            usize eol = source.find_first_of("\r\n", pos);
            RF_CORE_ASSERT(eol != std::string::npos, "Syntax error");
            usize begin = pos + length + 1;
            std::string type = source.substr(begin, eol - begin);

            usize next = source.find_first_not_of("\r\n", eol);
            pos = source.find(token, next);
            sources[ShaderTypeFromString(type)] =
                source.substr(next, pos - (next == std::string::npos ? source.size() - 1 : next));
        }

        Compile(sources);

        auto slash = filepath.find_last_of("/\\");
        slash = slash == std::string::npos ? 0 : slash + 1;
        auto dot = filepath.rfind('.');
        auto count = dot == std::string::npos ? filepath.size() - slash : dot - slash;
        mName = filepath.substr(slash, count);
    }

    Shader::~Shader()
    {
        glDeleteProgram(mRendererID);
    }

    void Shader::Bind() const
    {
        glUseProgram(mRendererID);
    }

    void Shader::Unbind() const
    {
        glUseProgram(0);
    }

    void Shader::SetInt(const std::string& name, int value) const
    {
        glUniform1i(GetUniformLocation(name), value);
    }

    void Shader::SetIntArray(const std::string& name, int* values, unsigned int count) const
    {
        glUniform1iv(GetUniformLocation(name), count, values);
    }

    void Shader::SetFloat(const std::string& name, float value) const
    {
        glUniform1f(GetUniformLocation(name), value);
    }

    void Shader::SetFloat2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2f(GetUniformLocation(name), value.x, value.y);
    }

    void Shader::SetFloat3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
    }

    void Shader::SetFloat4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
    }

    void Shader::SetMat3(const std::string& name, const glm::mat3& value) const
    {
        glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
    {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    Ref<Shader> Shader::Create(const std::string& name, const std::string& vertex, const std::string& fragment)
    {
        return CreateRef<Shader>(name, vertex, fragment);
    }

    Ref<Shader> Shader::Create(const std::string& filepath)
    {
        return CreateRef<Shader>(filepath);
    }

    void Shader::Compile(const std::unordered_map<u32, std::string>& shaderSources)
    {
        GLuint program = glCreateProgram();
        std::vector<GLenum> shaderIDs;

        for (const auto& [type, source] : shaderSources)
        {
            GLuint shader = glCreateShader(type);
            const GLchar* sourceCStr = source.c_str();
            glShaderSource(shader, 1, &sourceCStr, nullptr);
            glCompileShader(shader);

            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
                glDeleteShader(shader);

                RF_CORE_ERROR("Shader compilation failed: {}", infoLog.data());
                break;
            }

            glAttachShader(program, shader);
            shaderIDs.push_back(shader);
        }

        glLinkProgram(program);

        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, static_cast<int*>(&isLinked));
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
            glDeleteProgram(program);

            for (auto id : shaderIDs)
                glDeleteShader(id);

            RF_CORE_ERROR("Shader linking failed: {}", infoLog.data());
            return;
        }

        for (auto id : shaderIDs)
        {
            glDetachShader(program, id);
            glDeleteShader(id);
        }

        mRendererID = program;
    }

    i32 Shader::GetUniformLocation(const std::string& name) const
    {
        if (mUniformLocationCache.contains(name))
            return mUniformLocationCache[name];

        int location = glGetUniformLocation(mRendererID, name.c_str());
        mUniformLocationCache[name] = location;
        return location;
    }
} // namespace RF
