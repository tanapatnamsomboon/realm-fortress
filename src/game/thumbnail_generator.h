/**
 * @file thumbnail_generator.h
 * @brief
 * @date 12/17/2025
 */

#pragma once

#include "core/base.h"
#include "renderer/framebuffer.h"
#include "renderer/model.h"

namespace RealmFortress
{
    class ThumbnailGenerator
    {
    public:
        static void Init(u32 thumbnail_size = 128);
        static void Shutdown();

        static Ref<Framebuffer> GenerateThumbnail(const Ref<Model>& model);
        static u32 GetThumbnail(const std::string& model_path);

        static void ClearCache();

    private:
        static void RenderModelToFramebuffer(const Ref<Model>& model);

    private:
        inline static Ref<Shader> sThumbnailShader;
        inline static u32 sThumbnailSize{ 128 };
        inline static std::unordered_map<std::string, Ref<Framebuffer>> sThumbnailCache;
    };
} // namespace RealmFortress
