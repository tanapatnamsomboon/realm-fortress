/**
 * @file model_manager.h
 * @brief
 * @date 12/12/2025
 */

#pragma once

#include "core/base.h"
#include "renderer/model.h"
#include <string>
#include <unordered_map>

namespace RealmFortress
{
    class ModelManager
    {
    public:
        static Ref<Model> Load(const std::string& path);
        static Ref<Model> Get(const std::string& path);
        static bool Exists(const std::string& path);
        static void Clear();
        static usize GetCachedCount();

    private:
        static std::unordered_map<std::string, Ref<Model>> sModels;
    };
} // namespace RealmFortress
