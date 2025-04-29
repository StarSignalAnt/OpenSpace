#include "Importer.h"
#include <assimp/Importer.hpp>       // C++ importer interface
#include <assimp/scene.h>            // Output data structure
#include <assimp/postprocess.h>      // Post processing flags
#include <iostream>
#include "Mesh3DBuffer.h"
#include "NodeEntity.h"
#include "RenderMaterial.h"
#include "Texture2D.h"

NodeEntity* ProcessNode(aiNode* node, const aiScene* scene, const std::vector<Mesh3DBuffer*>& meshes) {
    NodeEntity* entity = new NodeEntity();

    // Set entity name (optional)
    entity->SetName(node->mName.C_Str());

    // Add meshes to entity
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        unsigned int meshIndex = node->mMeshes[i];
        if (meshIndex < meshes.size()) {
            entity->AddMesh(meshes[meshIndex]);  // You may need to define AddMesh
        }
    }

    // Recursively process children
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        NodeEntity* child = ProcessNode(node->mChildren[i], scene, meshes);
        entity->AddNode(child);  // Assuming NodeEntity supports AddChild
    }

    return entity;
}

NodeEntity* Importer::ImportEntity(std::string path) {

    Assimp::Importer importer;

    // Path to your 3D model
    const std::string modelPath = path;

    // Read the file
    const aiScene* scene = importer.ReadFile(modelPath,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace);

    // Check if import succeeded
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    std::cout << "Meshes:" << scene->mNumMeshes << std::endl;

    std::vector<Mesh3DBuffer*> meshes;
    std::vector<RenderMaterial*> mats;

    for (int i = 0;i < scene->mNumMaterials;i++) {

        auto mat = scene->mMaterials[i];
        auto rmat = new RenderMaterial;
        aiString path;
        if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
            std::string texturePath = path.C_Str();

            // Load your texture here using your own loader
            Texture2D* tex = new Texture2D(texturePath); // implement this function yourself
            rmat->SetColor(tex);
        }
        if (mat->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS) {
            std::string texturePath = path.C_Str();

            // Load your texture here using your own loader
            Texture2D* tex = new Texture2D(texturePath); // implement this function yourself
            rmat->SetNormal(tex);
        }
        mats.push_back(rmat);

    }

    for (int i = 0; i < scene->mNumMeshes;i++) {

        aiMesh* mesh = scene->mMeshes[i];
        std::cout << "Mesh " << i << ": " << mesh->mNumVertices << " vertices" << std::endl;
        Mesh3DBuffer* buf = new Mesh3DBuffer;

        for (int v = 0;v < mesh->mNumVertices;v++) {
            
            Vertex3D vert;

            vert.position = float3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
            vert.uv = float3(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y, 0);
            vert.color = float4(1, 1, 1, 1);
            vert.normal = float3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
            vert.bitangent = float3(mesh->mBitangents[v].x, mesh->mBitangents[v].y, mesh->mBitangents[v].z);
            vert.tangent = float3(mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z);
            buf->AddVertex(vert);



        }

        for (int f = 0;f < mesh->mNumFaces;f++) {

            Tri3D tri;

            tri.indices.x = mesh->mFaces[f].mIndices[0];
            tri.indices.y = mesh->mFaces[f].mIndices[2];
            tri.indices.z = mesh->mFaces[f].mIndices[1];

            buf->AddTriangle(tri);

        }

        buf->Finalize();
        RenderMaterial* mat = new RenderMaterial;
        buf->SetMaterial(mats[mesh->mMaterialIndex]);
        meshes.push_back(buf);


    }

    NodeEntity* ent = ProcessNode(scene->mRootNode, scene, meshes);
    return ent;


}