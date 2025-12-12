/**
 * @file building.h
 * @brief
 * @date 12/12/2025
 */

#pragma once

#include "core/base.h"
#include "game/system/coordinate.h"
#include "renderer/model.h"
#include <string>

namespace RealmFortress
{
    enum class BuildingType : u8
    {
        None = 0,

        TownHall,
        Castle,
        House,

        Barracks,
        ArcheryRange,
        Stables,

        Farm,
        LumberMill,
        Mine,
        Market,

        Wall,
        Tower,
        Gate,

        Count
    };

    enum class Faction : u8
    {
        Neutral = 0,
        Blue,
        Red,
        Green,
        Yellow,

        Count
    };

    class Building
    {
    public:
        Building() = default;
        Building(BuildingType type, Faction faction, const Coordinate& coord);

        BuildingType GetType() const { return mType; }
        Faction GetFaction() const { return mFaction; }
        const Coordinate& GetCoordinate() const { return mCoordinate; }

        void SetModel(const Ref<Model>& model) { mModel = model; }
        Ref<Model> GetModel() const { return mModel; }

        glm::mat4 GetTransform() const;

        bool IsConstructed() const { return mConstructed; }
        void SetConstructed(bool constructed) { mConstructed = constructed; }

    private:
        BuildingType mType{ BuildingType::None };
        Faction mFaction{ Faction::Neutral };
        Coordinate mCoordinate{};
        Ref<Model> mModel{ nullptr };
        bool mConstructed{ true };
    };

    const char* BuildingTypeToString(BuildingType type);
    const char* FactionToString(Faction faction);
    std::string BuildingTypeToModelPath(BuildingType type, Faction faction);
}
