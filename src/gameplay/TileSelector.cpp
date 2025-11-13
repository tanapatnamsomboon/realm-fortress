/**
 * @file TileSelector.cpp
 * @brief
 * @date 11/12/2025
 */

#include "TileSelector.h"
#include "gameplay/HexGrid.h"
#include "scene/Camera.h"
#include "renderer/Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/gl.h>

namespace RealmFortress
{
    TileSelector::TileSelector()
    {
        m_Highlight = std::make_unique<HighlightRenderer>();
    }

    void TileSelector::Update(const Camera& camera, const HexGrid& grid, float mouseX, float mouseY, int screenW, int screenH)
    {
        // ray from screen to world
        glm::vec3 rayOrigin = camera.Position();
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 proj = camera.GetProjectionMatrix();

        float ndcX = 2.0f * mouseX / screenW - 1.0f;
        float ndcY = 1.0f - 2.0f * mouseY / screenH;

        glm::vec4 rayClip{ ndcX, ndcY, -1.0f, 1.0f };
        glm::vec4 rayEye = glm::inverse(proj) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));

        // simple ray-plane intersect (Y=0 plane)
        float t = -rayOrigin.y / rayWorld.y;
        glm::vec3 hit = rayOrigin + rayWorld * t;

        // find the nearest tile by center distance
        const HexTile* nearest = nullptr;
        float minDist = 99999.9f;

        for (const auto& tile : grid.GetTiles())
        {
            float dist = glm::length(tile.WorldPosition - hit);
            if (dist < minDist && dist < 1.5f) // within 1 tile radius
            {
                nearest = &tile;
                minDist = dist;
            }
        }

        if (nearest)
            m_Selected = nearest;
        else
            m_Selected.reset();
    }

    void TileSelector::RenderHighlight(const glm::mat4& vp) const
    {
        if (!m_Selected.has_value()) return;

        const HexTile* tile = *m_Selected;
        m_Highlight->Draw(tile->WorldPosition, vp);
    }
} // namespace RealmFortress
