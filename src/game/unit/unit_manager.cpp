/**
 * @file unit_manager.cpp
 * @brief
 * @date 12/14/2025
 */

#include "core/pch.h"
#include "unit_manager.h"
#include "core/logger.h"
#include "renderer/model_cache.h"

namespace RealmFortress
{
    u32 UnitManager::SpawnUnit(UnitType type, Faction faction, const Coordinate& coord)
    {
        u32 id = mNextUnitId++;

        Unit unit(type, faction, coord);

        auto model = GetOrLoadModel(type, faction);
        if (model)
        {
            unit.SetModel(model);
        }

        mUnits[id] = std::move(unit);

        RF_CORE_INFO("Spawned {} ({}) at ({}, {})",
                   UnitTypeToString(type),
                   FactionToString(faction),
                   coord.Q, coord.R);

        return id;
    }

    bool UnitManager::RemoveUnit(u32 id)
    {
        auto it = mUnits.find(id);
        if (it != mUnits.end())
        {
            if (mSelectedUnitId.has_value() && mSelectedUnitId.value() == id)
            {
                mSelectedUnitId = std::nullopt;
            }
            mUnits.erase(it);
            return true;
        }
        return false;
    }

    void UnitManager::Clear()
    {
        mUnits.clear();
        mSelectedUnitId = std::nullopt;
    }

    Unit* UnitManager::GetUnit(u32 id)
    {
        auto it = mUnits.find(id);
        return it != mUnits.end() ? &it->second : nullptr;
    }

    const Unit* UnitManager::GetUnit(u32 id) const
    {
        auto it = mUnits.find(id);
        return it != mUnits.end() ? &it->second : nullptr;
    }

    std::vector<Unit*> UnitManager::GetUnitsAt(const Coordinate& coord)
    {
        std::vector<Unit*> result;
        for (auto& unit : mUnits | std::views::values)
        {
            if (unit.GetCoordinate() == coord)
            {
                result.push_back(&unit);
            }
        }
        return result;
    }

    void UnitManager::Update(f32 deltaTime)
    {
        for (auto& unit : mUnits | std::views::values)
        {
            unit.Update(deltaTime);
        }
    }

    void UnitManager::Draw(const Ref<Shader>& shader) const
    {
        if (!shader) return;

        shader->Bind();

        for (const auto& unit : mUnits | std::views::values)
        {
            auto model = unit.GetModel();
            if (model)
            {
                model->Draw(shader, unit.GetTransform());
            }
        }
    }

    void UnitManager::SelectUnit(u32 id)
    {
        if (mUnits.contains(id))
        {
            mSelectedUnitId = id;
        }
    }

    void UnitManager::ClearSelection()
    {
        mSelectedUnitId = std::nullopt;
    }

    Unit* UnitManager::GetSelectedUnit()
    {
        if (mSelectedUnitId.has_value())
        {
            return GetUnit(mSelectedUnitId.value());
        }
        return nullptr;
    }

    Ref<Model> UnitManager::GetOrLoadModel(UnitType type, Faction faction) const
    {
        u64 key = MakeKey(type, faction);

        auto it = mUnitModelCache.find(key);
        if (it != mUnitModelCache.end())
        {
            return it->second;
        }

        std::string path = UnitTypeToModelPath(type, faction);
        Ref<Model> model = ModelCache::Load(path);

        if (model)
        {
            mUnitModelCache[key] = model;
        }

        return model;
    }

    u64 UnitManager::MakeKey(UnitType type, Faction faction) const
    {
        return (static_cast<u64>(type) << 32) | static_cast<u64>(faction);
    }
} // namespace RealmFortress
