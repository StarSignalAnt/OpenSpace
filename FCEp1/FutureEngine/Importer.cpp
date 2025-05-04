#include "Importer.h"
#include <assimp/Importer.hpp>       // C++ importer interface
#include <assimp/scene.h>            // Output data structure
#include <assimp/postprocess.h>      // Post processing flags
#include <iostream>
#include "Mesh3DBuffer.h"
#include "NodeEntity.h"
#include "MaterialPBR3D.h"
#include "MaterialDepth3D.h"
#include "MaterialPBRS3D.h"
#include "MaterialDepthS3D.h"

#include "Texture2D.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp> // For glm::decompose
#include <glm/gtx/quaternion.hpp>       // For quaternion to matrix
#include <glm/gtx/transform.hpp>    
#include "NodeSkeletalEntity.h"
#include <iostream>
#include <filesystem>
#include "Animator.h"
#include "Animation.h"
namespace fs = std::filesystem;
using namespace Diligent;
std::string getFileNameWithoutExtension(const std::string& fileName) {
    std::filesystem::path filePath(fileName);
    return filePath.stem().string();
}
std::string ExtractFilename(const std::string& path) {
    // Find the last occurrence of the directory separator
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash == std::string::npos) {
        // No directory separator found, return the entire path
        return path;
    }
    else {
        // Extract the substring after the last directory separator
        return path.substr(lastSlash + 1);
    }
}

std::string getDirectoryPath(const std::string& fullPath) {
    size_t lastSlashPos = fullPath.find_last_of("/\\");
    if (lastSlashPos == std::string::npos) {
        // No slash found, return an empty string or handle appropriately
        return "";
    }
    return fullPath.substr(0, lastSlashPos + 1);
}


std::string extractFilenameAlone(const std::string& filePath) {
    // Find last occurrence of '/' or '\\' to handle both Unix and Windows paths
    size_t lastSlashIndex = filePath.find_last_of("/\\");

    // Extract substring starting right after the slash to the end
    std::string filename = (lastSlashIndex != std::string::npos)
        ? filePath.substr(lastSlashIndex + 1)
        : filePath;

    // Find last dot (.) to exclude extension
    size_t lastDotIndex = filename.find_last_of('.');

    // Extract substring up to the last dot if extension exists
    if (lastDotIndex != std::string::npos) {
        filename = filename.substr(0, lastDotIndex);
    }

    return filename;
}


std::string getFilename(const std::string& fullPath) {
    // Find the last occurrence of either '/' or '\\'
    size_t lastSlashPos = fullPath.find_last_of("/\\");

    if (lastSlashPos == std::string::npos) {
        // No slash found, the input is a filename itself
        return fullPath;
    }

    // Extract the filename from the full path
    return fullPath.substr(lastSlashPos + 1);
}
NodeEntity* ProcessNode(aiNode* node, const aiScene* scene, const std::vector<Mesh3DBuffer*>& meshes) {
    NodeEntity* entity = new NodeEntity();

    // Set entity name (optional)
    entity->SetName(node->mName.C_Str());

    aiMatrix4x4 aiTransform = node->mTransformation;
    glm::mat4 transform(
        aiTransform.a1, aiTransform.b1, aiTransform.c1, aiTransform.d1,
        aiTransform.a2, aiTransform.b2, aiTransform.c2, aiTransform.d2,
        aiTransform.a3, aiTransform.b3, aiTransform.c3, aiTransform.d3,
        aiTransform.a4, aiTransform.b4, aiTransform.c4, aiTransform.d4
    );

    glm::vec3 scale, translation, skew;
    glm::vec4 perspective;
    glm::quat orientation;

    glm::decompose(transform, scale, orientation, translation, skew, perspective);
    scale = glm::vec3(1, 1, 1);

    entity->SetPosition(translation); // if you have SetPosition
    entity->SetScale(scale);          // if you have SetScale
    entity->SetRotation(glm::toMat4(orientation));
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
        auto rmat = new MaterialPBR3D;
        aiString path;
        if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
            std::string texturePath = path.C_Str();

            // Load your texture here using your own loader


            Texture2D* tex = new Texture2D(texturePath,true); // implement this function yourself
            rmat->SetTexture(tex, 0);
        }
        if (mat->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS) {
            std::string texturePath = path.C_Str();

            // Load your texture here using your own loader
            Texture2D* tex = new Texture2D(texturePath,true); // implement this function yourself
            rmat->SetTexture(tex, 1);
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
            buf->AddVertex(vert,false);



        }

        for (int f = 0;f < mesh->mNumFaces;f++) {

            Tri3D tri;

            tri.indices.x = mesh->mFaces[f].mIndices[0];
            tri.indices.y = mesh->mFaces[f].mIndices[2];
            tri.indices.z = mesh->mFaces[f].mIndices[1];

            buf->AddTriangle(tri);

        }

        buf->Finalize();
        
        buf->SetMaterial(mats[mesh->mMaterialIndex]);
        buf->SetDepthMaterial(new MaterialDepth3D);
        meshes.push_back(buf);


    }

    NodeEntity* ent = ProcessNode(scene->mRootNode, scene, meshes);
    return ent;


}

NodeSkeletalEntity* Importer::ImportSkeletal(std::string path) {

    Assimp::Importer importer;

    // Define import flags (e.g., to triangulate polygons)
    unsigned int flags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_GenNormals;

    // Load the scene from the file
    const aiScene* scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Error: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    NodeSkeletalEntity* result = new NodeSkeletalEntity;

    //result->SetResourcePath(path);


    std::string path_alone = getDirectoryPath(path);

    std::string mat_path = path_alone + "Material/";

    int b = 6;
    fs::create_directory(mat_path);


    std::vector<RenderMaterial*> materials;


    for (int i = 0; i < scene->mNumMaterials; i++) {

        auto mat = scene->mMaterials[i];
        auto name = std::string(mat->GetName().C_Str());

        auto v_mat = new MaterialPBRS3D;

        std::string check = mat_path + name + ".material";

        //auto lmat = Engine::FindActiveMaterial(check);


        //if (lmat != nullptr) {
            //     materials.push_back(lmat);
            //     continue;
        //}

        //if (VFile::Exists(check.c_str())) {

            //   auto r = v_mat->LoadMaterial(check);

               //v_mat->SetDiffuse(r->GetDiffuse());
              // v_mat->SetNormals(r->GetNormal());
             //  v_mat->SetSpecular(r->GetSpecular());

             //  materials.push_back(v_mat);
              // Engine::m_ActiveMaterials.push_back(v_mat);
             //  continue;

        //}

        //Engine::m_ActiveMaterials.push_back(v_mat);
        materials.push_back(v_mat);

        for (unsigned int j = 0; j < mat->GetTextureCount(aiTextureType_DIFFUSE); ++j) {
            aiString str;

            mat->GetTexture(aiTextureType_DIFFUSE, j, &str);
            str = ExtractFilename(str.C_Str()).c_str();
            //v_mat->SetColor();

            v_mat->SetTexture(new Texture2D(path_alone + getFilename(std::string(str.C_Str())), true), 0);
            //GLuint textureID = LoadTextureFromFile(directory + '/' + str.C_Str());
            std::cout << "Loaded diffuse texture ID:  for texture " << str.C_Str() << std::endl;
        }

        // Load specular textures
        for (unsigned int j = 0; j < mat->GetTextureCount(aiTextureType_SPECULAR); ++j) {
            aiString str;
            mat->GetTexture(aiTextureType_SPECULAR, j, &str);
            str = ExtractFilename(str.C_Str()).c_str();
           // v_mat->SetSpecular(new Texture2D(path_alone + std::string(str.C_Str()), true));
            //GLuint textureID = LoadTextureFromFile(directory + '/' + str.C_Str());
            std::cout << "Loaded specular texture ID: for texture " << str.C_Str() << std::endl;
        }

        // Load normal textures (height maps in Assimp terminology)
        for (unsigned int j = 0; j < mat->GetTextureCount(aiTextureType_NORMALS); ++j) {
            aiString str;

            mat->GetTexture(aiTextureType_NORMALS, j, &str);
            str = ExtractFilename(str.C_Str()).c_str();
            v_mat->SetTexture(new Texture2D(path_alone + std::string(str.C_Str()), true),1);

            //GLuint textureID = LoadTextureFromFile(directory + '/' + str.C_Str());
            std::cout << "Loaded normal texture ID: for texture " << str.C_Str() << std::endl;
        }

        int b = 5;

        //v_mat->SaveMaterial(mat_path + name + ".material");

    }


    for (int i = 0; i < scene->mNumMeshes;i++) {

        //Mesh3D* mesh = new Mesh3D((NodeEntity*)result);
        auto mesh = result->ProcessMesh(scene->mMeshes[i], (aiScene*)scene, false);

        mesh->Finalize();


        mesh->SetMaterial(materials[scene->mMeshes[i]->mMaterialIndex]);
        mesh->SetDepthMaterial(new MaterialDepthS3D);
        //mesh->SetDepthMaterial(new MaterialActorDepth);


    }


    auto anim = new Animation((aiScene*)scene, scene->mAnimations[0], result);
    auto animator = new Animator(anim);

    for (int i = 1; i < scene->mNumAnimations; i++) {

        auto new_anim = new Animation((aiScene*)scene, scene->mAnimations[i], result);

    }

    result->SetAnimator(animator);


    return result;



}