/**
 * @file renderer.cpp
 * @brief
 * @date 11/9/2025
 */

#include "renderer.h"
#include "core/logger.h"
#include <glad/gl.h>

namespace RealmFortress
{
    Renderer::SceneData Renderer::sSceneData;
    Renderer::Statistics Renderer::sStats;

    void Renderer::Init()
    {
        RF_CORE_INFO("Initializing Renderer (OpenGL)");

        // enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // enable depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // enable face culling for performance
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }

    void Renderer::Shutdown()
    {
        RF_CORE_INFO("Shutting down Renderer");
    }

    void Renderer::OnWindowResize(u32 width, u32 height)
    {
        SetViewport(0, 0, width, height);
    }

    void Renderer::BeginFrame()
    {
        ResetStats();
    }

    void Renderer::EndFrame()
    {
        // TODO: frame cleanup
    }

    void Renderer::BeginScene(const Camera& camera)
    {
        sSceneData.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }

    void Renderer::EndScene()
    {
        // TODO: frame cleanup
    }

    void Renderer::DrawMesh(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
    {
        shader->Bind();
        shader->SetMat4("uViewProjection", sSceneData.ViewProjectionMatrix);
        shader->SetMat4("uTransform", transform);

        DrawIndexed(vertexArray);
    }

    void Renderer::DrawIndexed(const Ref<VertexArray>& vertexArray, u32 indexCount)
    {
        vertexArray->Bind();
        u32 count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();

        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

        sStats.DrawCalls++;
        sStats.IndexCount += count;
        sStats.TriangleCount += count / 3;
    }

    void Renderer::DrawArrays(const Ref<VertexArray>& vertexArray, u32 vertexCount)
    {
        vertexArray->Bind();
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        sStats.DrawCalls++;
        sStats.VertexCount += vertexCount;
        sStats.TriangleCount += vertexCount / 3;
    }

    void Renderer::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void Renderer::SetViewport(u32 x, u32 y, u32 width, u32 height)
    {
        glViewport(x, y, width, height);
    }

    void Renderer::SetBlendMode(bool enabled)
    {
        if (enabled)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }

    void Renderer::SetDepthTest(bool enabled)
    {
        if (enabled)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
    }

    void Renderer::SetCullFace(bool enabled)
    {
        if (enabled)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
    }

    void Renderer::SetWireframe(bool enabled)
    {
        glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
    }

    void Renderer::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
} // namespace RealmFortress
