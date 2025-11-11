/**
 * @file Model.cpp
 * @brief
 * @date 11/9/2025
 */

#include "Model.h"
#include <glad/gl.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace RealmFortress
{
    static std::vector<Vertex> ToVertices(const aiMesh* mesh)
    {
        std::vector<Vertex> vertices;
        vertices.reserve(mesh->mNumVertices);
        for (unsigned i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex vertex;
            vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            if (mesh->HasNormals())
                vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
            if (mesh->HasTextureCoords(0))
                vertex.UV = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            vertices.push_back(vertex);
        }

        return vertices;
    }

    static std::vector<unsigned> ToIndices(const aiMesh* mesh)
    {
        std::vector<unsigned> indices;
        indices.reserve(mesh->mNumFaces * 3);
        for (unsigned f = 0; f < mesh->mNumFaces; ++f)
        {
            const aiFace& face = mesh->mFaces[f];
            for (unsigned j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }

        return indices;
    }

    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, unsigned int diffuse)
        : m_Diffuse(diffuse), m_IndexCount(static_cast<int>(indices.size()))
    {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, Position)));
        glEnableVertexAttribArray(1); // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, Normal)));
        glEnableVertexAttribArray(2); // uv
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, UV)));
        glBindVertexArray(0);
    }

    Mesh::~Mesh()
    {
        if (m_EBO) glDeleteBuffers(1, &m_EBO);
        if (m_VBO) glDeleteBuffers(1, &m_VBO);
        if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    }

    void Mesh::Draw() const
    {
        if (m_Diffuse)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_Diffuse);
        }
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    Model::Model(const std::string& path)
    {
        Load(path);
    }

    void Model::Draw() const
    {
        for (const auto& mesh : m_Meshes)
            mesh.Draw();
    }

    void Model::Load(const std::string& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenNormals |
            aiProcess_ImproveCacheLocality | aiProcess_JoinIdenticalVertices
        );
        if (!scene || !scene->mRootNode)
        {
            throw std::runtime_error("Assimp load failed: " + std::string(importer.GetErrorString()));
        }

        m_Directory = std::filesystem::path(path).parent_path().string();
        std::vector<unsigned int> materialDiffuse(scene->mNumMaterials, 0);

        for (unsigned i = 0; i < scene->mNumMaterials; ++i)
        {
            aiString tpath;
            if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &tpath) == AI_SUCCESS)
            {
                std::filesystem::path texPath = std::filesystem::path(m_Directory) / std::filesystem::path(tpath.C_Str());
                materialDiffuse[i] = LoadTexture2D(texPath.string(), true);
                if (!m_DefaultDiffuse)
                    m_DefaultDiffuse = materialDiffuse[i];
            }
        }

        for (unsigned i = 0; i < scene->mNumMeshes; ++i)
        {
            const aiMesh* mesh = scene->mMeshes[i];
            unsigned diffuse = 0;
            if (mesh->mMaterialIndex < materialDiffuse.size())
                diffuse = materialDiffuse[mesh->mMaterialIndex];
            m_Meshes.emplace_back(ToVertices(mesh), ToIndices(mesh), diffuse);
        }
    }

    unsigned int LoadTexture2D(const std::string& path, bool srgb)
    {
        stbi_set_flip_vertically_on_load(true);
        int width, height, channel;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channel, 4);
        if (!data)
        {
            std::cerr << "[Texture] Failed " << path << std::endl;
            return 0;
        }
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);

        return texture;
    }
} // namespace RealmFortress
