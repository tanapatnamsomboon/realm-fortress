/**
 * @file unit.h
 * @brief
 * @date 12/6/2025
 */

#pragma once

#include "core/base.h"
#include "game/resource/resource_type.h"
#include "game/hex/hex_coordinate.h"
#include "game/hex/hex_pathfinding.h"
#include "game/faction.h"
#include "renderer/model.h"
#include "renderer/shader.h"
#include <string>
#include <vector>
#include <queue>

namespace RealmFortress
{
    enum class UnitType : u8
    {
        None = 0,

        // Civilian
        Worker,
        Merchant,

        // Infantry
        Spearman,
        Swordsman,

        // Ranged
        Archer,
        Crossbowman,

        // Cavalry
        Knight,
        HorseArcher,

        // Siege
        Catapult,
        Cannon,

        // Naval
        Ship,
        Warship,

        Count
    };

    inline const char* UnitTypeToString(UnitType type)
    {
        switch (type)
        {
        case UnitType::Worker:       return "Worker";
        case UnitType::Merchant:     return "Merchant";
        case UnitType::Spearman:     return "Spearman";
        case UnitType::Swordsman:    return "Swordsman";
        case UnitType::Archer:       return "Archer";
        case UnitType::Crossbowman:  return "Crossbowman";
        case UnitType::Knight:       return "Knight";
        case UnitType::HorseArcher:  return "Horse Archer";
        case UnitType::Catapult:     return "Catapult";
        case UnitType::Cannon:       return "Cannon";
        case UnitType::Ship:         return "Ship";
        case UnitType::Warship:      return "Warship";
        default: return "None";
        }
    }

    struct UnitStats
    {
        i32 MaxHealth   = 100;
        i32 Attack      = 10;
        i32 Defense     = 5;
        f32 MoveSpeed   = 2.0f; // hexes per second
        i32 AttackRange = 1;  // in hexes
        i32 VisionRange = 3;  // in hexes
    };

    struct UnitDefinition
    {
        UnitType                    Type = UnitType::None;
        std::string                 Name;
        std::string                 Description;
        std::string                 ModelPath;
        UnitStats                   Stats;
        std::vector<ResourceAmount> Cost;

        UnitDefinition() = default;
        UnitDefinition(
            UnitType                              type,
            const std::string&                    name,
            const std::string&                    desc,
            const std::string&                    modelPattern,
            const UnitStats&                      stats,
            std::initializer_list<ResourceAmount> cost
        )
            : Type(type), Name(name), Description(desc)
            , ModelPath(modelPattern), Stats(stats), Cost(cost)
        {
        }

        std::string GetModelPath(Faction faction) const;
    };

    class Unit
    {
    public:
        Unit(
            UnitType              type,
            const HexCoordinate&  position,
            Faction               faction,
            const UnitDefinition& definition
        );
        ~Unit();

        UnitType GetType() const { return mType; }
        Faction GetFaction() const { return mFaction; }
        const HexCoordinate& GetPosition() const { return mPosition; }
        const std::string& GetName() const { return mDefinition.Name; }
        const UnitDefinition& GetDefinition() const { return mDefinition; }

        void Update(Timestep ts, const HexMap& hexMap);
        void Draw(const Ref<Shader>& shader);

        // Movement
        void MoveTo(const HexCoordinate& target, const HexMap& hexMap);
        void Stop();
        bool IsMoving() const { return mIsMoving; }
        const std::vector<HexCoordinate>& GetPath() const { return mPath; }

        // Combat
        void Attack(Unit* target);
        void TakeDamage(i32 damage);
        bool IsAlive() const { return mCurrentHealth > 0; }

        i32 GetCurrentHealth() const { return mCurrentHealth; }
        i32 GetMaxHealth() const { return mDefinition.Stats.MaxHealth; }
        f32 GetHealthPercent() const { return static_cast<f32>(mCurrentHealth) / mDefinition.Stats.MaxHealth; }

        // Selection
        void SetSelected(bool selected) { mIsSelected = selected; }
        bool IsSelected() const { return mIsSelected; }

        // Rotation
        void SetRotation(f32 rotation) { mRotation = rotation; }
        f32 GetRotation() const { return mRotation; }

    private:
        void UpdateMovement(Timestep ts, const HexMap& hexMap);
        void UpdateRotation();

    private:
        UnitType mType;
        Faction mFaction;
        HexCoordinate mPosition;
        UnitDefinition mDefinition;
        Ref<Model> mModel;

        // Stats
        i32 mCurrentHealth;

        // Movement
        bool mIsMoving = false;
        std::vector<HexCoordinate> mPath;
        size_t mCurrentPathIndex = 0;
        glm::vec3 mWorldPosition;
        glm::vec3 mTargetWorldPosition;
        f32 mMoveProgress = 0.0f;

        // Rendering
        f32 mRotation = 0.0f;
        bool mIsSelected = false;
    };

    class UnitManager
    {
    public:
        UnitManager();

        void Initialize();
        void Update(Timestep ts, const HexMap& map);

        const UnitDefinition* GetUnitDefinition(UnitType type) const;
        const std::vector<UnitDefinition>& GetAllDefinitions() const { return mDefinitions; }

        Unit* SpawnUnit(
            UnitType             type,
            const HexCoordinate& position,
            Faction              faction,
            ResourceManager&     resources
        );
        void RemoveUnit(Unit* unit);

        Unit* GetUnitAt(const HexCoordinate& position);
        const Unit* GetUnitAt(const HexCoordinate& position) const;

        const std::vector<Scope<Unit>>& GetAllUnits() const { return mUnits; }
        std::vector<Unit*> GetUnitsOfFaction(Faction faction);

        void DrawUnits(const Ref<Shader>& shader);

        void SetResourceManager(ResourceManager* resourceManager) { mResourceManager = resourceManager; }

    private:
        std::vector<UnitDefinition> mDefinitions;
        std::vector<Scope<Unit>> mUnits;
        std::unordered_map<HexCoordinate, Unit*> mUnitMap;
        ResourceManager* mResourceManager = nullptr;
    };
} // namespace RealmFortress
