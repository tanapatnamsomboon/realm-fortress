/**
 * @file renderer.cpp
 * @brief
 * @date 11/9/2025
 */

#include "renderer.h"
#include "renderer/vertex_array.h"

namespace RF
{
    Scope<Renderer::SceneData> Renderer::sSceneData = CreateScope<SceneData>();

    void Renderer::Init()
    {
        RenderCommand::Init();
    }

    void Renderer::Shutdown()
    {
    }

    void Renderer::OnWindowResize(u32 width, u32 height)
    {
        RenderCommand::SetViewport(0, 0, width, height);
    }

    void Renderer::BeginScene(const Camera& camera)
    {
        sSceneData->mViewProjectionMatrix = camera.GetProjection();
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
    {
        shader->Bind();
        shader->SetMat4("uViewProjection", sSceneData->mViewProjectionMatrix);
        shader->SetMat4("uTransform", transform);

        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }
} // namespace RF
