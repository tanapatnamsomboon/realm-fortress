/**
 * @file model.h
 * @brief
 * @date 11/21/2025
 */

#pragma once

#include "core/base.h"
#include "renderer/mesh.h"
#include "renderer/shader.h"
#include <assimp/scene.h>
#include <string>
#include <vector>

namespace RF
{
    class Model
    {
    public:
        Model(const std::string& path);

        void Draw(const Ref<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));

        const std::vector<Mesh>& GetMeshes() const { return mMeshes; }

    private:
        void LoadModel(const std::string& path);
        void ProcessNode(aiNode* node, const aiScene* scene);

        Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

        std::vector<Ref<Texture2D>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& name);

    private:
        std::vector<Mesh> mMeshes;
        std::string mDirectory;
    };
} // namespace RF
