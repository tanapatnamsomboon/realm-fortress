/**
 * @file farm.h
 * @brief
 * @date 12/18/2025
 */

#pragma once

#include "core/base.h"
#include "game/building/building.h"
#include "game/resource/resource.h"

namespace RealmFortress
{
    class Farm final : public Building
    {
    public:
        Farm(const Coordinate& coordinate);

        void OnUpdate(Timestep ts) override;
        void OnInspected() override;
        void OnPlaced(const Map& map) override;
        void OnDestroyed(const Map& map) override;
        void OnMapChanged(const Map& map) override;

        bool CanPlace(const Coordinate& coord, const Map& map) const override;

    private:
        f32 mTime;
        f32 mCycleTime;

        // TODO: implement own buffer
    };
} // namespace RealmFortress
