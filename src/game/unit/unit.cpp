/**
 * @file unit.cpp
 * @brief
 * @date 12/6/2025
 */

#include "core/pch.h"
#include "unit.h"
#include "game/hex/hex_map.h"
#include "core/logger.h"
#include <glm/gtc/matrix_transform.hpp>
#include <format>

namespace RealmFortress
{
    std::string UnitDefinition::GetModelPath(Faction faction) const
    {
        std::string path = ModelPath;
        std::string factionStr = FactionToColorName(faction);

        size_t pos = path.find("{faction}");
        while (pos != std::string::npos)
        {
            path.replace(pos, 9, factionStr);
            pos = path.find("{faction}", pos + factionStr.length());
        }

        return path;
    }

    // ========== Unit ==========

    Unit::Unit(UnitType type, const HexCoordinate& position, Faction faction,
               const UnitDefinition& definition)
                   : mType(type), mFaction(faction), mPosition(position), mDefinition(definition)
                   , mCurrentHealth(definition.Stats.MaxHealth)
    {
        mWorldPosition = position.ToWorldPosition();
        mTargetWorldPosition = mWorldPosition;

        std::string modelPath = definition.GetModelPath(faction);

        try
        {
            mModel = CreateRef<Model>(modelPath);
            RF_TRACE("Loaded unit model: {}", modelPath);
        }
        catch (const std::exception& e)
        {
            RF_CORE_ERROR("Failed to load unit model '{}': {}", modelPath, e.what());
        }
    }

    Unit::~Unit() = default;

    void Unit::Update(Timestep ts, const HexMap& hexMap)
    {
        if (mIsMoving)
        {
            UpdateMovement(ts, hexMap);
        }
    }

    void Unit::Draw(const Ref<Shader>& shader)
    {
        if (!mModel) return;

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), mWorldPosition);
        transform = glm::rotate(transform, glm::radians(mRotation), glm::vec3(0.0f, 1.0f, 0.0f));

        mModel->Draw(shader, transform);
    }

    void Unit::MoveTo(const HexCoordinate& target, const HexMap& hexMap)
    {
        if (target == mPosition)
        {
            mIsMoving = false;
            return;
        }

        mPath = HexPathfinder::FindPath(mPosition, target, hexMap);

        if (!mPath.empty())
        {
            mIsMoving = true;
            mCurrentPathIndex = 0;
            RF_TRACE("Unit moving to ({}, {}) - {} steps", target.q, target.r, mPath.size());
        }
        else
        {
            RF_WARN("No path found to ({}, {})", target.q, target.r);
            mIsMoving = false;
        }
    }

    void Unit::Stop()
    {
        mIsMoving = false;
        mPath.clear();
        mCurrentPathIndex = 0;
    }

    void Unit::Attack(Unit* target)
    {
        if (!target || !target->IsAlive())
            return;

        i32 damage = mDefinition.Stats.Attack - target->mDefinition.Stats.Defense;
        damage = std::max(damage, 1);

        target->TakeDamage(damage);

        RF_TRACE("{} attacks {} for {} damage", GetName(), target->GetName(), damage);

    }

    void Unit::TakeDamage(i32 damage)
    {
        mCurrentHealth -= damage;
        if (mCurrentHealth < 0)
            mCurrentHealth = 0;

        if (!IsAlive())
        {
            RF_INFO("{} has been defeated!", GetName());
        }
    }

    void Unit::UpdateMovement(Timestep ts, const HexMap& hexMap)
    {
        if (mPath.empty() || mCurrentPathIndex >= mPath.size())
        {
            mIsMoving = false;
            return;
        }

        const HexCoordinate& targetHex = mPath[mCurrentPathIndex];
        mTargetWorldPosition = targetHex.ToWorldPosition();

        glm::vec3 direction = mTargetWorldPosition - mWorldPosition;
        f32 distance = glm::length(direction);

        if (distance < 0.01f)
        {
            mWorldPosition = mTargetWorldPosition;
            mPosition = targetHex;
            mCurrentPathIndex++;

            if (mCurrentPathIndex >= mPath.size())
            {
                mIsMoving = false;
                RF_TRACE("Unit reached destination ({}, {})", mPosition.q, mPosition.r);
            }
        }
        else
        {
            f32 moveDistance = mDefinition.Stats.MoveSpeed * static_cast<f32>(ts);
            if (moveDistance > distance)
                moveDistance = distance;

            mWorldPosition += glm::normalize(direction) * moveDistance;

            if (glm::length(direction) > 0.001f)
            {
                mRotation = glm::degrees(std::atan2(direction.x, direction.z));
            }
        }
    }

    void Unit::UpdateRotation()
    {
    }

    // ========== UnitManager ==========

    UnitManager::UnitManager() = default;

    void UnitManager::Initialize()
    {
        RF_CORE_INFO("Initializing Unit Manager");

        mDefinitions = {
            // Civilian
            UnitDefinition(
                UnitType::Worker,
                "Worker",
                "Gathers resources and builds structures",
                "assets/objects/units/{faction}/{faction}_full.gltf",
                UnitStats{ 50, 5, 2, 2.5f, 1, 2 },
                { ResourceAmount(ResourceType::Food, 50) }
            ),

            // Infantry
            UnitDefinition(
                UnitType::Spearman,
                "Spearman",
                "Basic infantry unit with spear",
                "assets/objects/units/{faction}/spear_{faction}_full.gltf",
                UnitStats{ 100, 15, 10, 2.0f, 1, 3 },
                {
                    ResourceAmount(ResourceType::Food, 50),
                    ResourceAmount(ResourceType::Wood, 25),
                    ResourceAmount(ResourceType::Iron, 10)
                }
            ),

            UnitDefinition(
                UnitType::Swordsman,
                "Swordsman",
                "Heavy infantry with sword and shield",
                "assets/objects/units/{faction}/sword_{faction}_full.gltf",
                UnitStats{ 120, 20, 15, 1.8f, 1, 3 },
                {
                    ResourceAmount(ResourceType::Food, 60),
                    ResourceAmount(ResourceType::Iron, 20),
                    ResourceAmount(ResourceType::Gold, 10)
                }
            ),

            // Ranged
            UnitDefinition(
                UnitType::Archer,
                "Archer",
                "Ranged unit with bow",
                "assets/objects/units/{faction}/bow_{faction}_full.gltf",
                UnitStats{ 70, 18, 5, 2.0f, 3, 4 },
                {
                    ResourceAmount(ResourceType::Food, 40),
                    ResourceAmount(ResourceType::Wood, 30),
                    ResourceAmount(ResourceType::Gold, 5)
                }
            ),

            // Cavalry
            UnitDefinition(
                UnitType::Knight,
                "Knight",
                "Heavily armored cavalry",
                "assets/objects/units/{faction}/horse_{faction}_full.gltf",
                UnitStats{ 150, 25, 20, 4.0f, 1, 4 },
                {
                    ResourceAmount(ResourceType::Food, 80),
                    ResourceAmount(ResourceType::Iron, 30),
                    ResourceAmount(ResourceType::Gold, 25)
                }
            ),

            // Siege
            UnitDefinition(
                UnitType::Catapult,
                "Catapult",
                "Siege weapon for destroying buildings",
                "assets/objects/units/{faction}/catapult_{faction}_full.gltf",
                UnitStats{ 80, 50, 5, 1.0f, 5, 3 },
                {
                    ResourceAmount(ResourceType::Wood, 100),
                    ResourceAmount(ResourceType::Iron, 50),
                    ResourceAmount(ResourceType::Gold, 50)
                }
            )
        };

        RF_CORE_INFO("Loaded {} unit definitions", mDefinitions.size());
    }

    void UnitManager::Update(Timestep ts, const HexMap& map)
    {
        for (auto& unit : mUnits)
        {
            unit->Update(ts, map);
        }

        std::erase_if(mUnits,
            [this](const Scope<Unit>& unit) {
                if (!unit->IsAlive())
                {
                    mUnitMap.erase(unit->GetPosition());
                    return true;
                }
                return false;
            });

        mUnitMap.clear();
        for (const auto& unit : mUnits)
        {
            if (!unit->IsMoving())
            {
                mUnitMap[unit->GetPosition()] = unit.get();
            }
        }
    }

    const UnitDefinition* UnitManager::GetUnitDefinition(UnitType type) const
    {
        for (const auto& def : mDefinitions)
        {
            if (def.Type == type)
                return &def;
        }
        return nullptr;
    }

    Unit* UnitManager::SpawnUnit(UnitType type, const HexCoordinate& position,
                                 Faction faction, ResourceManager& resources)
    {
        const UnitDefinition* def = GetUnitDefinition(type);
        if (!def)
        {
            RF_WARN("Unknown unit type: {}", static_cast<int>(type));
            return nullptr;
        }

        if (mUnitMap.contains(position))
        {
            RF_WARN("Cannot spawn unit - tile occupied at ({}, {})", position.q, position.r);
            return nullptr;
        }

        if (!resources.CanAfford(def->Cost))
        {
            RF_WARN("Not enough resources to spawn '{}'", def->Name);
            return nullptr;
        }

        resources.SpendResources(def->Cost);

        auto unit = CreateScope<Unit>(type, position, faction, *def);
        Unit* pUnit = unit.get();

        mUnits.push_back(std::move(unit));
        mUnitMap[position] = pUnit;

        RF_INFO("Spawned '{}' at ({}, {}) for {}", def->Name, position.q, position.r,
               FactionToString(faction));

        return pUnit;
    }

    void UnitManager::RemoveUnit(Unit* unit)
    {
        if (!unit) return;

        mUnitMap.erase(unit->GetPosition());

        auto it = std::ranges::find_if(mUnits, [unit](const Scope<Unit>& u) { return u.get() == unit; });

        if (it != mUnits.end())
        {
            RF_INFO("Removed unit at ({}, {})", unit->GetPosition().q, unit->GetPosition().r);
            mUnits.erase(it);
        }
    }

    Unit* UnitManager::GetUnitAt(const HexCoordinate& position)
    {
        auto it = mUnitMap.find(position);
        return (it != mUnitMap.end()) ? it->second : nullptr;
    }

    const Unit* UnitManager::GetUnitAt(const HexCoordinate& position) const
    {
        auto it = mUnitMap.find(position);
        return (it != mUnitMap.end()) ? it->second : nullptr;
    }

    std::vector<Unit*> UnitManager::GetUnitsOfFaction(Faction faction)
    {
        std::vector<Unit*> result;
        for (const auto& unit : mUnits)
        {
            if (unit->GetFaction() == faction)
                result.push_back(unit.get());
        }
        return result;
    }

    void UnitManager::DrawUnits(const Ref<Shader>& shader)
    {
        for (const auto& unit : mUnits)
        {
            unit->Draw(shader);
        }
    }
} // namespace RealmFortress
