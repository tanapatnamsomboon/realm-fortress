/**
 * @file lumbermill.h
 * @brief
 * @date 12/18/2025
 */

#pragma once

#include "core/base.h"
#include "game/building/building.h"
#include "game/resource/resource.h"

namespace RealmFortress
{
    class Lumbermill final : public Building
    {
    public:
        Lumbermill(const Coordinate& coordinate);

        void OnUpdate(Timestep ts) override;
        void OnInspected() override;
        void OnPlaced(const Map& map) override;
        void OnDestroyed(const Map& map) override;
        void OnMapChanged(const Map& map) override;

        bool CanPlace(const Coordinate& coord, const Map& map) const override;

    private:
        void UpdateEfficiency(const Map& map);
        f32 CalculateResourceBonus(const Map& map);

    private:
        f32 mTime;
        f32 mBaseCycleTime;
        f32 mEfficiency;
        f32 mProgress;

        // TODO: implement own buffer
    };
} // namespace RealmFortress

