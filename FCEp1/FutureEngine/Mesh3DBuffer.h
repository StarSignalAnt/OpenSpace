#pragma once
#include <vector>

#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "RenderStateNotationLoader.h"
#include "BasicMath.hpp"
#include "FutureApp.h"
#include "RenderMaterial.h"

using namespace Diligent;

struct Vertex3D {

	float3 position;
	float3 uv;
	float4 color;
	float3 normal;
	float3 tangent;
	float3 bitangent;
    float4 bone_ids; //19
    float4 bone_weights;

};

struct Tri3D {

	int3 indices;

};

class Mesh3DBuffer
{
public:

    Mesh3DBuffer() {

    };

    void AddVertex(Vertex3D& vertex,bool reset) {
        if (reset) {
            //vertex.bone_ids = float4(-1, -1, -1, -1);
            //vertex.bone_weights = float4(0, 0, 0, 0);
            vertex.bone_ids = float4(-1, -1, -1, -1);
            vertex.bone_weights = float4(0, 0, 0, 0);
        }
        vertices.push_back(vertex);

    }

    void AddTriangle(const Tri3D& tri) {
        triangles.push_back(tri);
    }

    Vertex3D GetVertex(int num) {
        return vertices[num];
    }

    void SetVertex(int id,Vertex3D vertex) {
        vertices[id] = vertex;
    }

    void Finalize() {
        using namespace Diligent;

        auto device = FutureApp::m_Inst->GetDevice();


        if (vertices.empty() || triangles.empty()) return;

        // Create vertex buffer
        {
            BufferDesc vbDesc;
            vbDesc.Name = "Vertex Buffer";
            vbDesc.Usage = USAGE_IMMUTABLE;
            vbDesc.BindFlags = BIND_VERTEX_BUFFER;
            vbDesc.Size = static_cast<Uint32>(vertices.size() * sizeof(Vertex3D));

            BufferData vbData;
            vbData.pData = vertices.data();
            vbData.DataSize = vbDesc.Size;

            device->CreateBuffer(vbDesc, &vbData, &vertexBuffer);
        }

        // Create index buffer
        {
            std::vector<uint32_t> indexData;
            indexData.reserve(triangles.size() * 3);
            for (const auto& tri : triangles) {
                indexData.push_back(tri.indices[0]);
                indexData.push_back(tri.indices[1]);
                indexData.push_back(tri.indices[2]);
            }

            BufferDesc ibDesc;
            ibDesc.Name = "Index Buffer";
            ibDesc.Usage = USAGE_IMMUTABLE;
            ibDesc.BindFlags = BIND_INDEX_BUFFER;
            ibDesc.Size = static_cast<Uint32>(indexData.size() * sizeof(uint32_t));

            BufferData ibData;
            ibData.pData = indexData.data();
            ibData.DataSize = ibDesc.Size;

            device->CreateBuffer(ibDesc, &ibData, &indexBuffer);
            indexCount = static_cast<uint32_t>(indexData.size());
        }

        // Optionally clear CPU-side memory if no longer needed
        //vertices.clear();
       // triangles.clear();

    }


    // Getters
    Diligent::IBuffer* GetVertexBuffer() const { return vertexBuffer.RawPtr(); }
    Diligent::IBuffer* GetIndexBuffer() const { return indexBuffer.RawPtr(); }
    int VertexCount() const { return m_VertexCount; }
    int IndexCount() const { return indexCount; }
    int VertexCapacity() const { return m_VertexCapacity; }
    int IndexCapacity() const { return m_TriCapacity; }
    void SetMaterial(RenderMaterial* mat) {
        m_Material = mat;
    }
    RenderMaterial* GetMaterial() {
        return m_Material;
    }
    void SetDepthMaterial(RenderMaterial* material) {
        m_DepthMaterial = material;
    }
    RenderMaterial* GetDepthMaterial()
    {
        return m_DepthMaterial;
    }


private:
    void CreateDynamicBuffers(int vertexCapacity, int triangleCapacity);

    // GPU Resources
    Diligent::RefCntAutoPtr<Diligent::IBuffer> vertexBuffer;
    Diligent::RefCntAutoPtr<Diligent::IBuffer> indexBuffer;
    // CPU-side data
    std::vector<Vertex3D> vertices;
    std::vector<Tri3D> triangles;
    uint32_t indexCount = 0;
    // Capacity and tracking
    bool m_IsDynamic;
    int m_VertexCapacity;
    int m_TriCapacity;
    int m_VertexCount;
    int m_TriCount;

    // Direct access to mapped memory
    void* m_MappedVertexData;
    void* m_MappedIndexData;
    RenderMaterial* m_Material;
    RenderMaterial* m_DepthMaterial;
};

