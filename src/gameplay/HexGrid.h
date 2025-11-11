/**
 * @file HexGrid.h
 * @brief
 * @date 11/9/2025
 */

#pragma once

#include "gameplay/HexTile.h"
#include <glm/mat4x4.hpp>
#include <vector>

namespace RealmFortress
{
    class Shader;
    class Model;

    /**
     * @class HexGrid
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class HexGrid
    {
    public:
        HexGrid(int width, int height, float scale = 1.0f);

        void SetModel(TileKind kind, const Model* model, float scale = 1.0f);
        void GenerateFlat(TileKind kind);

        void Render(const Shader& shader, const glm::mat4& vp) const;

        static glm::vec3 AxialToWorld(int q, int r, float radius);

    private:
        int m_Width{ 0 }, m_Height{ 0 };
        float m_Scale{ 1.0f };
        std::vector<HexTile> m_Tiles;
        const Model* m_ModelGrass{ nullptr };
        const Model* m_ModelWater{ nullptr };
        const Model* m_ModelRoad{ nullptr };
        const Model* m_ModelCoast{ nullptr };
        float m_ScaleGrass{ 1.0f }, m_ScaleWater{ 1.0f }, m_ScaleRoad{ 1.0f }, m_ScaleCoast{ 1.0f };
    };
} // namespace RealmFortress
