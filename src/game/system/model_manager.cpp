/**
 * @file model_manager.cpp
 * @brief
 * @date 12/12/2025
 */

#include "core/pch.h"
#include "model_manager.h"

namespace RealmFortress
{
    std::unordered_map<std::string, Ref<Model>> ModelManager::sModels;

    Ref<Model> ModelManager::Load(const std::string& path)
    {
        if (sModels.contains(path))
        {
            RF_CORE_TRACE("Model cache hit: {}", path);
            return sModels[path];
        }

        try
        {
            auto model = CreateRef<Model>(path);
            sModels[path] = model;
            RF_CORE_INFO("Model loaded and cached: {}", path);
            return model;
        }
        catch (const std::exception& e)
        {
            RF_CORE_ERROR("Failed to load model {}: {}", path, e.what());
            return nullptr;
        }
    }

    Ref<Model> ModelManager::Get(const std::string& path)
    {
        auto it = sModels.find(path);
        return it != sModels.end() ? it->second : nullptr;
    }

    bool ModelManager::Exists(const std::string& path)
    {
        return sModels.contains(path);
    }

    void ModelManager::Clear()
    {
        RF_CORE_INFO("Clearing model cache ({} models)", sModels.size());
        sModels.clear();
    }

    usize ModelManager::GetCachedCount()
    {
        return sModels.size();
    }
} // namespace RealmFortress
