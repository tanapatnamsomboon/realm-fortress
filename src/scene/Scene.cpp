/**
 * @file Scene.cpp
 * @brief
 * @date 11/9/2025
 */

#include "Scene.h"
#include "renderer/Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/gl.h>

namespace RealmFortress
{
    void Scene::AddEntity(const Entity& e)
    {
        m_Entities.push_back(e);
    }

    void Scene::Render(const Shader& shader, const glm::mat4& vp)
    {
        shader.Use();
        for (const auto& e : m_Entities) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), e.Position);
            model = glm::scale(model, e.Scale);
            shader.SetMat4("uMVP", vp * model);
            shader.SetVec3("uColor", e.Color);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
} // RealmFortress
