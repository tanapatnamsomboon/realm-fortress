/**
 * @file model.cpp
 * @brief
 * @date 11/22/2025
 */

#include "renderer/model.h"
#include "core/logger.h"
#include "renderer/texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace RF
{
    Model::Model(const std::string& path)
    {
        LoadModel(path);
    }

    void Model::Draw(const Ref<Shader>& shader, const glm::mat4& transform)
    {
        for (auto& mesh : mMeshes)
        {
            mesh.Draw(shader, transform);
        }
    }

    void Model::LoadModel(const std::string& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace |
            aiProcess_GenNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            RF_CORE_ERROR("Failed to load model: {}", importer.GetErrorString());
            return;
        }

        mDirectory = path.substr(0, path.find_last_of('/'));
        ProcessNode(scene->mRootNode, scene);
    }

    void Model::ProcessNode(aiNode* node, const aiScene* scene)
    {
        for (u32 i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            mMeshes.push_back(ProcessMesh(mesh, scene));
        }

        for (u32 i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<u32> indices;
        std::vector<Ref<Texture2D>> textures;

        for (u32 i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            vertex.mPosition = glm::vec3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );

            if (mesh->HasNormals())
            {
                vertex.mNormal = glm::vec3(
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z
                );
            }
            else
            {
                vertex.mNormal = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            if (mesh->mTextureCoords[0])
            {
                vertex.mTexCoords = glm::vec2(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                );
            }
            else
            {
                vertex.mTexCoords = glm::vec2(0.0f);
            }

            vertices.push_back(vertex);
        }

        for (u32 i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (u32 j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Ref<Texture2D>> diffuse = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuse.begin(), diffuse.end());

        std::vector<Ref<Texture2D>> specular = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specular.begin(), specular.end());

        return Mesh(vertices, indices, textures);
    }

    std::vector<Ref<Texture2D>> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& name) {
        std::vector<Ref<Texture2D>> textures;
        for (u32 i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);

            std::string texturePath = mDirectory + "/" + std::string(str.C_Str());
            auto texture = Texture2D::Create(texturePath);
            textures.push_back(texture);
        }
        return textures;
    }
} // namespace RF
