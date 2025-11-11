/**
 * @file HexGrid.cpp
 * @brief
 * @date 11/9/2025
 */

#include "HexGrid.h"
#include "renderer/Shader.h"
#include "renderer/Model.h"
#include <glm/gtc/matrix_transform.hpp>

namespace RealmFortress
{
    HexGrid::HexGrid(int width, int height, float scale)
        : m_Width(width), m_Height(height), m_Scale(scale)
    {
    }

    void HexGrid::SetModel(TileKind kind, const Model *model, float scale)
    {
        switch (kind)
        {
            case TileKind::Grass:
                m_ModelGrass = model;
                m_ScaleGrass = scale;
                break;
            case TileKind::Water:
                m_ModelWater = model;
                m_ScaleWater = scale;
                break;
            case TileKind::Road:
                m_ModelRoad = model;
                m_ScaleRoad = scale;
                break;
            case TileKind::Coast:
                m_ModelCoast = model;
                m_ScaleCoast = scale;
                break;
        }
    }

    void HexGrid::GenerateFlat(TileKind kind)
    {
        m_Tiles.clear();
        m_Tiles.reserve(m_Width * m_Height);
        int q0 = -m_Width / 2;
        int r0 = -m_Height / 2;
        for (int r = 0; r < m_Height; ++r)
        {
            for (int q = 0; q < m_Width; ++q)
            {
                HexTile tile;
                tile.Coord = { q0 + q, r0 + r };
                tile.Kind = kind;
                tile.WorldPosition = AxialToWorld(tile.Coord.Q, tile.Coord.R, m_Scale);
                switch (kind)
                {
                    case TileKind::Grass:
                        tile.Model = m_ModelGrass;
                        tile.ModelScale = m_ScaleGrass;
                        break;
                    case TileKind::Water:
                        tile.Model = m_ModelWater;
                        tile.ModelScale = m_ScaleGrass;
                        break;
                    case TileKind::Road:
                        tile.Model = m_ModelRoad;
                        tile.ModelScale = m_ScaleRoad;
                        break;
                    case TileKind::Coast:
                        tile.Model = m_ModelCoast;
                        tile.ModelScale = m_ScaleCoast;
                        break;
                }
                m_Tiles.push_back(tile);
            }
        }
    }

    void HexGrid::Render(const Shader& shader, const glm::mat4& vp) const
    {
        shader.Use();
        for (const auto& tile : m_Tiles) {
            if (!tile.Model) continue;
            glm::mat4 M = glm::translate(glm::mat4(1.0f), tile.WorldPosition);
            M = glm::scale(M, glm::vec3(tile.ModelScale));
            shader.SetMat4("uMVP", vp * M);
            shader.SetMat4("uModel", M);
            tile.Model->Draw();
        }
    }

    glm::vec3 HexGrid::AxialToWorld(int q, int r, float radius)
    {
        constexpr float sqrt3 = 1.7320508075688772f;
        float x = sqrt3 * (q + r * 0.5f) * radius;
        float z = 1.5f * r * radius;
        return { x, 0.0f, z };
    }
} // namespace RealmFortress
