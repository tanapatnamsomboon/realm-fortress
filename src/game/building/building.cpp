/**
 * @file building.cpp
 * @brief
 * @date 11/29/2025
 */

#include "core/pch.h"
#include "building.h"
#include "core/logger.h"

namespace RealmFortress
{
    std::string BuildingDefinition::GetModelPath(Faction faction) const
    {
        std::string path = ModelPath;
        std::string factionStr = FactionToColorName(faction);

        size_t pos = path.find("{faction}");
        while (pos != std::string::npos)
        {
            path.replace(pos, 9, factionStr); // 9 = length of "{faction}"
            pos = path.find("{faction}", pos + factionStr.length());
        }

        return path;
    }

    Building::Building(BuildingType type, const HexCoordinate& position,
                       Faction faction, const BuildingDefinition& definition)
        : mType(type), mFaction(faction), mPosition(position), mDefinition(definition)
        , mConstructed(false), mConstructionProgress(0.0f),mRotation(0.0f)
    {
        std::string modelPath = definition.GetModelPath(faction);

        try
        {
            mModel = CreateRef<Model>(definition.ModelPath);
            RF_TRACE("Loaded building model: {}", modelPath);
        }
        catch (const std::exception& e)
        {
            RF_CORE_ERROR("Failed to load building model: {0}", e.what());
        }
    }

    Building::~Building() = default;

    void Building::Update(Timestep ts)
    {
        if (!mConstructed && mConstructionProgress < 1.0f)
        {
            if (mDefinition.Cost.BuildTime > 0.0f)
            {
                mConstructionProgress += static_cast<f32>(ts) / mDefinition.Cost.BuildTime;

                if (mConstructionProgress >= 1.0f)
                {
                    mConstructionProgress = 1.0f;
                    mConstructed = true;
                    RF_INFO("Building '{}' construction complete!", mDefinition.Name);
                }
            }
            else
            {
                mConstructed = true;
                mConstructionProgress = 1.0f;
            }
        }
    }

    void Building::Draw(const Ref<Shader>& shader)
    {
        if (!mModel) return;

        glm::vec3 worldPos = mPosition.ToWorldPosition();
        worldPos.y = 0.0f;

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos);
        transform = glm::rotate(transform, glm::radians(mRotation), glm::vec3(0.0f, 1.0f, 0.0f));

        if (!mConstructed)
        {
            f32 scale = 0.5f + 0.5f * mConstructionProgress;
            transform = glm::scale(transform, glm::vec3(scale, mConstructionProgress, scale));
        }

        mModel->Draw(shader, transform);
    }

    std::string Building::GetProductionId() const
    {
        return std::format("building_{}_{}", static_cast<int>(mType), reinterpret_cast<uintptr_t>(this));
    }

    BuildingManager::BuildingManager() = default;

    void BuildingManager::Initialize()
    {
        RF_CORE_INFO("Initializing Building Manager");

        mDefinitions = {
            // Townhall - main building
            BuildingDefinition(
                BuildingType::Townhall,
                "Town Hall",
                "Central administrative building",
                "assets/objects/buildings/{faction}/building_townhall_{faction}.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 200),
                    ResourceAmount(ResourceType::Stone, 150)
                }, 5.0f),
                ResourceProduction({}, {}, 0.0f),
                5 // population capacity
            ),

            // Housing
            BuildingDefinition(
                BuildingType::Home,
                "Home",
                "Houses for colonists",
                "assets/objects/buildings/{faction}/building_home_A_{faction}.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 50),
                    ResourceAmount(ResourceType::Stone, 20)
                }, 3.0f),
                ResourceProduction({}, {}, 0.0f),
                4 // population capacity
            ),

            // Resource production
            BuildingDefinition(
                BuildingType::Lumbermill,
                "Lumbermill",
                "Produces wood from nearby trees",
                "assets/objects/buildings/{faction}/building_lumbermill_{faction}.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 75),
                    ResourceAmount(ResourceType::Stone, 30)
                }, 4.0f),
                ResourceProduction({
                    ResourceAmount(ResourceType::Wood, 5)
                }, {}, 2.0f) // Produces 5 wood every 2 seconds
            ),

            BuildingDefinition(
                BuildingType::Mine,
                "Mine",
                "Extracts stone and iron ore",
                "assets/objects/buildings/{faction}/building_mine_{faction}.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 50),
                    ResourceAmount(ResourceType::Stone, 20)
                }, 4.0f),
                ResourceProduction({
                    ResourceAmount(ResourceType::Stone, 4),
                    ResourceAmount(ResourceType::Iron, 1)
                }, {}, 2.0f)
            ),

            BuildingDefinition(
                BuildingType::Farm,
                "Farm",
                "Grows food for population",
                "assets/objects/buildings/neutral/building_grain.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 60),
                    ResourceAmount(ResourceType::Stone, 15)
                }, 3.0f),
                ResourceProduction({
                    ResourceAmount(ResourceType::Food, 10)
                }, {}, 3.0f)
            ),

            BuildingDefinition(
                BuildingType::Windmill,
                "Windmill",
                "Grinds grain into food",
                "assets/objects/buildings/{faction}/building_windmill_{faction}.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 80),
                    ResourceAmount(ResourceType::Stone, 40)
                }, 4.0f),
                ResourceProduction({
                    ResourceAmount(ResourceType::Food, 15)
                }, {}, 4.0f)
            ),

            // Economy
            BuildingDefinition(
                BuildingType::Market,
                "Market",
                "Trading post that generates gold",
                "assets/objects/buildings/{faction}/building_market_{faction}.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 80),
                    ResourceAmount(ResourceType::Stone, 50),
                    ResourceAmount(ResourceType::Gold, 10)
                }, 5.0f),
                ResourceProduction({
                    ResourceAmount(ResourceType::Gold, 2)
                }, {}, 5.0f)
            ),

            // Military
            BuildingDefinition(
                BuildingType::Barracks,
                "Barracks",
                "Trains soldiers",
                "assets/objects/buildings/{faction}/building_barracks_{faction}.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 100),
                    ResourceAmount(ResourceType::Stone, 80),
                    ResourceAmount(ResourceType::Iron, 20)
                }, 6.0f)
            ),

            BuildingDefinition(
                BuildingType::ArcheryRange,
                "Archery Range",
                "Trains archers",
                "assets/objects/buildings/{faction}/building_archery_range_{faction}.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 90),
                    ResourceAmount(ResourceType::Stone, 50)
                }, 5.0f)
            ),

            BuildingDefinition(
                BuildingType::Tower,
                "Tower",
                "Defense structure with archers",
                "assets/objects/buildings/{faction}/building_tower_A_{faction}.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 50),
                    ResourceAmount(ResourceType::Stone, 100),
                    ResourceAmount(ResourceType::Iron, 15)
                }, 5.0f)
            ),

            BuildingDefinition(
                BuildingType::Castle,
                "Castle",
                "Fortified stronghold",
                "assets/objects/buildings/{faction}/building_castle_{faction}.gltf",
                BuildingCost({
                    ResourceAmount(ResourceType::Wood, 200),
                    ResourceAmount(ResourceType::Stone, 300),
                    ResourceAmount(ResourceType::Iron, 50),
                    ResourceAmount(ResourceType::Gold, 100)
                }, 10.0f),
                ResourceProduction({}, {}, 0.0f),
                10 // population capacity
            )
        };

        RF_CORE_INFO("Loaded {0} building definitions", mDefinitions.size());
    }

    void BuildingManager::Update(Timestep ts)
    {
        for (auto& building : mBuildings)
        {
            building->Update(ts);

            if (building->IsConstructed() && mResourceManager)
            {
                const auto& prod = building->GetDefinition().Production;
                if (!prod.Production.empty() || !prod.Consumption.empty())
                {
                    mResourceManager->RegisterProducer(building->GetProductionId(), prod);
                }

                if (building->GetDefinition().PopulationProvided > 0)
                {

                }
            }
        }
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

    Building* BuildingManager::PlaceBuilding(BuildingType type, const HexCoordinate& position,
            Faction faction, ResourceManager& resources)
    {
        const BuildingDefinition* def = GetBuildingDefinition(type);
        if (!def)
        {
            RF_WARN("Unknown building type!");
            return nullptr;
        }

        if (!CanPlaceBuilding(type, position))
        {
            RF_WARN("Cannot place building at ({}, {})", position.q, position.r);
            return nullptr;
        }

        if (!resources.CanAfford(def->Cost.Resources))
        {
            RF_WARN("Not enough resources to build '{}'", def->Name);
            return nullptr;
        }

        resources.SpendResources(def->Cost.Resources);

        auto building = CreateScope<Building>(type, position, faction, *def);
        Building* pBuilding = building.get();

        mBuildings.push_back(std::move(building));
        mBuildingMap[position] = pBuilding;

        if (def->PopulationProvided > 0)
        {
            resources.AddPopulationCap(def->PopulationProvided);
        }

        RF_INFO("Placed '{}' at ({}, {}) for {}", def->Name, position.q, position.r, FactionToString(faction));

        return pBuilding;
    }

    void BuildingManager::RemoveBuilding(const HexCoordinate& position)
    {
        auto it = mBuildingMap.find(position);
        if (it == mBuildingMap.end())
            return;

        Building* building = it->second;

        if (mResourceManager)
        {
            mResourceManager->UnregisterProducer(building->GetProductionId());

            if (building->GetDefinition().PopulationProvided > 0)
            {
                mResourceManager->RemovePopulationCap(building->GetDefinition().PopulationProvided);
            }
        }

        mBuildingMap.erase(it);

        auto vecIt = std::ranges::find_if(mBuildings, [building](const Scope<Building>& b)
        {
            return b.get() == building;
        });

        if (vecIt != mBuildings.end())
        {
            RF_INFO("Removed building at ({0}, {1})", position.q, position.r);
            mBuildings.erase(vecIt);
        }
    }

    Building* BuildingManager::GetBuildingAt(const HexCoordinate& position)
    {
        auto it = mBuildingMap.find(position);
        return (it != mBuildingMap.end()) ? it->second : nullptr;
    }

    const Building* BuildingManager::GetBuildingAt(const HexCoordinate& position) const
    {
        auto it = mBuildingMap.find(position);
        return (it != mBuildingMap.end()) ? it->second : nullptr;
    }

    std::vector<Building*> BuildingManager::GetBuildingOfFaction(Faction faction)
    {
        std::vector<Building*> result;
        for (const auto& building : mBuildings)
        {
            if (building->GetFaction() == faction)
                result.push_back(building.get());
        }
        return result;
    }

    void BuildingManager::DrawBuildings(const Ref<Shader>& shader)
    {
        for (const auto& building : mBuildings)
        {
            building->Draw(shader);
        }
    }
} // namespace RealmFortress
