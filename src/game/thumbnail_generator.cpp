/**
 * @file thumbnail_generator.cpp
 * @brief
 * @date 12/17/2025
 */

#include "core/pch.h"
#include "thumbnail_generator.h"
#include "renderer/renderer.h"
#include "renderer/model_cache.h"
#include "core/application.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace RealmFortress
{
    void ThumbnailGenerator::Init(u32 thumbnail_size)
    {
        sThumbnailSize = thumbnail_size;
        sThumbnailShader = Shader::Create("assets/shaders/thumbnail.glsl");
    }

    void ThumbnailGenerator::Shutdown()
    {
        sThumbnailShader.reset();
        sThumbnailCache.clear();
    }

    Ref<Framebuffer> ThumbnailGenerator::GenerateThumbnail(const Ref<Model>& model)
    {
        if (!model || !sThumbnailShader)
            return nullptr;

        FramebufferSpecification spec;
        spec.Width = sThumbnailSize;
        spec.Height = sThumbnailSize;
        spec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };

        Ref<Framebuffer> framebuffer = Framebuffer::Create(spec);

        framebuffer->Bind();

        Renderer::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        Renderer::Clear();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(
            glm::vec3(2.0f, 2.0f, 2.0f),  // Eye position
            glm::vec3(0.0f, 0.0f, 0.0f),  // Look at a center
            glm::vec3(0.0f, 0.4f, 0.0f)   // Up
        );
        Renderer::SetSceneViewProjection(projection * view);

        sThumbnailShader->Bind();
        sThumbnailShader->SetInt("uTexture", 0);
        sThumbnailShader->SetInt("uHasTexture", 1);
        model->Draw(sThumbnailShader);
        sThumbnailShader->Unbind();

        framebuffer->Unbind();

        auto& window = Application::Get().GetWindow();
        Renderer::SetViewport(0, 0, window.GetWidth(), window.GetHeight());

        return framebuffer;
    }

    u32 ThumbnailGenerator::GetThumbnail(const std::string& model_path)
    {
        if (sThumbnailCache.contains(model_path))
        {
            return sThumbnailCache[model_path]->GetColorAttachmentRendererID();
        }

        auto model = ModelCache::Get(model_path);
        if (!model)
        {
            model = ModelCache::Load(model_path);
        }

        if (model)
        {
            Ref<Framebuffer> fbo = GenerateThumbnail(model);
            if (fbo)
            {
                sThumbnailCache[model_path] = fbo;
                return fbo->GetColorAttachmentRendererID();
            }
        }

        return 0;
    }

    void ThumbnailGenerator::ClearCache()
    {
        sThumbnailCache.clear();
    }

    void ThumbnailGenerator::RenderModelToFramebuffer(const Ref<Model>& model)
    {
    }
} // namespace RealmFortress
