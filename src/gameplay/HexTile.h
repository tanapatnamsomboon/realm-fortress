/**
 * @file HexTile.h
 * @brief
 * @date 11/9/2025
 */

#pragma once

#include <glm/glm.hpp>

namespace RealmFortress
{
    class Model;

    struct Axial
    {
        int Q{ 0 };
        int R{ 0 };
    };

    enum class TileKind : int
    {
        Grass,
        Water,
        Road,
        Coast,
    };

    struct HexTile
    {
        Axial        Coord;
        TileKind     Kind{ TileKind::Grass };
        glm::vec3    WorldPosition{ 0.0f };
        float        ModelScale{ 1.0f };
        const Model* Model;
    };
} // namespace RealmFortress
