/**
 * @file building.cpp
 * @brief
 * @date 11/29/2025
 */

#include "core/pch.h"
#include "building.h"
#include "core/logger.h"

namespace RF
{
    Building::Building(BuildingType type, const HexCoordinate& position, const BuildingDefinition& definition)
        : mType(type), mPosition(position), mDefinition(definition), mConstructed(true), mRotation(1.0f)
    {
        try
        {
            mModel = CreateRef<Model>(definition.ModelPath);
        }
        catch (const std::exception& e)
        {
            RF_CORE_ERROR("failed to load building model: {0}", e.what());
        }
    }

    void Building::Draw(const Ref<Shader>& shader)
    {
        if (!mModel) return;

        glm::vec3 worldPos = mPosition.ToWorldPosition();
        worldPos.y = 0.0f;

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos);
        transform = glm::rotate(transform, glm::radians(mRotation), glm::vec3(0.0f, 1.0f, 0.0f));

        mModel->Draw(shader, transform);
    }

    BuildingManager::BuildingManager()
    {
    }

    void BuildingManager::Initialize()
    {
        RF_CORE_INFO("initializing building manager");

        mDefinitions = {
            BuildingDefinition(
                BuildingType::Home,
                "Home",
                "Houses for colonists",
                "assets/objects/buildings/blue/building_home_A_blue.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 50),
                    ResourceAmount(ResourceType::Stone, 20)
                })
            ),
            BuildingDefinition(
                BuildingType::Lumbermill,
                "Lumbermill",
                "Produces wood from trees",
                "assets/objects/buildings/blue/building_lumbermill_blue.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 75),
                    ResourceAmount(ResourceType::Stone, 30)
                })
            ),
            BuildingDefinition(
                BuildingType::Mine,
                "Mine",
                "Extracts stone and ore",
                "assets/objects/buildings/blue/building_mine_blue.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 50),
                    ResourceAmount(ResourceType::Stone, 20),
                })
            ),
            BuildingDefinition(
                BuildingType::Farm,
                "Farm",
                "Trading post",
                "assets/objects/buildings/neutral/building_grain.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 50),
                    ResourceAmount(ResourceType::Stone, 20),
                })
            ),
            BuildingDefinition(
                BuildingType::Market,
                "Market",
                "Trading post",
                "assets/objects/buildings/blue/building_market_blue.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 80),
                    ResourceAmount(ResourceType::Stone, 50),
                    ResourceAmount(ResourceType::Gold, 10)
                })
            ),
            BuildingDefinition(
                BuildingType::Barracks,
                "Barracks",
                "Trains soldiers",
                "assets/objects/buildings/blue/building_barracks_blue.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 100),
                    ResourceAmount(ResourceType::Stone, 80),
                    ResourceAmount(ResourceType::Iron, 20)
                })
            ),
            BuildingDefinition(
                BuildingType::Tower,
                "Tower",
                "Defense structure",
                "assets/objects/buildings/blue/building_tower_A_blue.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 50),
                    ResourceAmount(ResourceType::Stone, 100),
                    ResourceAmount(ResourceType::Iron, 15)
                })
            )
        };

        RF_CORE_INFO("loaded {0} building definitions", mDefinitions.size());
    }

    const BuildingDefinition* BuildingManager::GetBuildingDefinition(BuildingType type) const
    {
        for (const auto& def : mDefinitions)
        {
            if (def.Type == type)
            {
                return &def;
            }
        }
        return nullptr;
    }

    bool BuildingManager::CanPlaceBuilding(BuildingType type, const HexCoordinate& position) const
    {
        if (mBuildingMap.contains(position))
        {
            return false;
        }
        return true;
    }

    Building* BuildingManager::PlaceBuilding(BuildingType type, const HexCoordinate& position, ResourceManager& resources)
    {
        const BuildingDefinition* def = GetBuildingDefinition(type);
        if (!def)
        {
            RF_WARN("unknown building type!");
            return nullptr;
        }

        if (!CanPlaceBuilding(type, position))
        {
            RF_WARN("cannot place building at this location!");
            return nullptr;
        }

        for (const auto& cost : def->Cost.Resources)
        {
            if (!resources.HasResource(cost.Type, cost.Amount))
            {
                RF_WARN("not enough resources to build {0}!", def->Name.c_str());
                return nullptr;
            }
        }

        for (const auto& cost : def->Cost.Resources)
        {
            resources.RemoveResource(cost.Type, cost.Amount);
        }

        auto building = CreateScope<Building>(type, position, *def);
        Building* pBuilding = building.get();

        mBuildings.push_back(std::move(building));
        mBuildingMap[position] = pBuilding;

        RF_INFO("placed {0} at ({1}, {2})", def->Name.c_str(), position.q, position.r);

        return pBuilding;
    }

    void BuildingManager::RemoveBuilding(const HexCoordinate& position)
    {
        auto it = mBuildingMap.find(position);
        if (it == mBuildingMap.end())
        {
            return;
        }

        Building* building = it->second;
        mBuildingMap.erase(it);

        auto vecIt = std::find_if(mBuildings.begin(), mBuildings.end(),
            [building](const Scope<Building>& b) { return b.get() == building; });

        if (vecIt != mBuildings.end())
        {
            RF_INFO("removed building at ({0}, {1})", position.q, position.r);
            mBuildings.erase(vecIt);
        }
    }

    Building* BuildingManager::GetBuildingAt(const HexCoordinate& position)
    {
        auto it = mBuildingMap.find(position);
        if (it != mBuildingMap.end())
        {
            return it->second;
        }
        return nullptr;
    }

    const Building* BuildingManager::GetBuildingAt(const HexCoordinate& position) const
    {
        auto it = mBuildingMap.find(position);
        if (it != mBuildingMap.end())
        {
            return it->second;
        }
        return nullptr;
    }

    void BuildingManager::DrawBuildings(const Ref<Shader>& shader)
    {
        for (const auto& building : mBuildings)
        {
            building->Draw(shader);
        }
    }
} // namespace RF
