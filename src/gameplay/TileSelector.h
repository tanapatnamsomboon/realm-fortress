/**
 * @file TileSelector.h
 * @brief
 * @date 11/12/2025
 */

#pragma once

#include "renderer/HighlightRenderer.h"
#include <glm/glm.hpp>
#include <optional>
#include <memory>

namespace RealmFortress
{
    class Camera;
    class HexGrid;
    struct HexTile;

    /**
     * @class TileSelector
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class TileSelector
    {
    public:
        TileSelector();

        void Update(const Camera& camera, const HexGrid& grid, float mouseX, float mouseY, int screenW, int screenH);

        std::optional<const HexTile*> GetSelected() const { return m_Selected; }
        void RenderHighlight(const glm::mat4& vp) const;

    private:
        std::optional<const HexTile*> m_Selected;
        std::unique_ptr<HighlightRenderer> m_Highlight;
    };
} // namespace RealmFortress
