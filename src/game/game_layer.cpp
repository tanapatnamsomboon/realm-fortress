/**
 * @file game_layer.cpp
 * @brief
 * @date 11/27/2025
 */

#include "core/pch.h"
#include "game_layer.h"
#include "core/application.h"
#include "renderer/renderer.h"

namespace RF
{
    GameLayer::GameLayer()
        : Layer("GameLayer"), mCameraController(1600.0f / 900.0f)
    {
    }

    void GameLayer::OnAttach()
    {
        RF_INFO("GameLayer attached");

        mShader = Shader::Create("assets/shaders/basic.glsl");

        mHexMap = CreateScope<HexMap>();
        mHexMap->Generate(5, 5);
    }

    void GameLayer::OnDetach()
    {
        RF_INFO("GameLayer detached");
    }

    void GameLayer::OnUpdate(Timestep ts)
    {
        mCameraController.OnUpdate(ts);

        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.15f, 1.0f });
        RenderCommand::Clear();

        Renderer::BeginScene(mCameraController.GetCamera());

        mShader->Bind();
        mShader->SetFloat3("uLightPos", glm::vec3(10.0f, 20.0f, 10.0f));
        mShader->SetFloat3("uViewPos", mCameraController.GetCamera().GetPosition());
        mShader->SetFloat3("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        mShader->SetFloat3("uTintColor", glm::vec3(1.0f, 1.0f, 1.0f));

        mHexMap->Draw(mShader);

        Renderer::EndScene();
    }

    void GameLayer::OnEvent(Event& event)
    {
        mCameraController.OnEvent(event);
    }
} // namespace RF
