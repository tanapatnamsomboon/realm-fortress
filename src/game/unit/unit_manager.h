/**
 * @file unit_manager.h
 * @brief
 * @date 12/14/2025
 */

#pragma once

#include "core/base.h"
#include "game/unit/unit.h"
#include "renderer/shader.h"
#include <vector>
#include <optional>

namespace RealmFortress
{
    class UnitManager
    {
    public:
        UnitManager() = default;

        u32 SpawnUnit(UnitType type, Faction faction, const Coordinate& coord);
        bool RemoveUnit(u32 id);
        void Clear();

        Unit* GetUnit(u32 id);
        const Unit* GetUnit(u32 id) const;
        std::vector<Unit*> GetUnitsAt(const Coordinate& coord);

        void Update(f32 deltaTime);
        void Draw(const Ref<Shader>& shader) const;

        usize GetUnitCount() const { return mUnits.size(); }

        void SelectUnit(u32 id);
        void ClearSelection();
        bool HasSelection() const { return mSelectedUnitId.has_value(); }
        std::optional<u32> GetSelectedUnitId() const { return mSelectedUnitId; }
        Unit* GetSelectedUnit();

    private:
        Ref<Model> GetOrLoadModel(UnitType type, Faction faction) const;
        u64 MakeKey(UnitType type, Faction faction) const;

    private:
        std::unordered_map<u32, Unit> mUnits;
        std::optional<u32> mSelectedUnitId;
        u32 mNextUnitId{ 1 };

        mutable std::unordered_map<u64, Ref<Model>> mUnitModelCache;
    };
} // namespace RealmFortress
