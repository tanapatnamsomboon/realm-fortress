/**
 * @file mine.h
 * @brief
 * @date 12/15/2025
 */

#pragma once

#include "core/base.h"
#include "game/building/building.h"
#include "game/resource/resource.h"

namespace RealmFortress
{
    class Mine final : public Building
    {
    public:
        Mine(const Coordinate& coordinate);

        void OnUpdate(Timestep ts) override;
        void OnPlaced() override;

        bool CanPlace(const Coordinate& coord, const Map& map) const override;

    private:
        f32 mTime;
        f32 mCycleTime;

        // TODO: implement own buffer
    };
} // namespace RealmFortress
