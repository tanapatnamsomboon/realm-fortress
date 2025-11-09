/**
 * @file Scene.h
 * @brief
 * @date 11/9/2025
 */

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace RealmFortress
{
    class Shader;

    struct Entity
    {
        std::string Name;
        glm::vec3 Position{ 0.0f };
        glm::vec3 Scale{ 1.0f };
        glm::vec3 Color{ 1.0f };
    };

    /**
     * @class Scene
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Scene
    {
    public:
        void AddEntity(const Entity& e);
        void Render(const Shader& shader, const glm::mat4& vp);

        std::vector<Entity>& GetEntities() { return m_Entities; }

    private:
        std::vector<Entity> m_Entities;
    };
} // RealmFortress
