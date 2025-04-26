#pragma once
#include <vector>

#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "RenderStateNotationLoader.h"
#include "BasicMath.hpp"

using namespace Diligent;

struct Vertex {
    float3 pos;
    float2 uv;
    float4 col;
    float3 realPos;
    float4 extra;
    float4 view;
};

struct Tri {
    int3 indices;
};

class MeshBuffer
{
public:
    MeshBuffer();
    MeshBuffer(int vertexCapacity, int triangleCapacity);

    // Vertex/triangle addition functions
    void AddVertex(Vertex vertex);
    void AddTriangle(Tri triangle);

    // Buffer management functions
    void Finalize();
    void Clear();

    // Advanced buffer management for optimal performance
    void BeginUpdate();  // Maps buffers for direct writing
    void EndUpdate();    // Unmaps buffers
    void UpdateBuffers(); // Manual buffer update

    // Buffer resize functions (automatically called when needed)
    void ResizeVertexBuffer(int newCapacity);
    void ResizeIndexBuffer(int newCapacity);

    // Getters
    RefCntAutoPtr<IBuffer> GetVertexBuffer() const { return m_VertexBuffer; }
    RefCntAutoPtr<IBuffer> GetIndexBuffer() const { return m_IndexBuffer; }
    int VertexCount() const { return m_VertexCount; }
    int IndexCount() const { return m_TriCount; }
    int VertexCapacity() const { return m_VertexCapacity; }
    int IndexCapacity() const { return m_TriCapacity; }

private:
    // Helper function for creating dynamic buffers
    void CreateDynamicBuffers(int vertexCapacity, int triangleCapacity);

    // GPU Resources
    RefCntAutoPtr<IBuffer> m_VertexBuffer;
    RefCntAutoPtr<IBuffer> m_IndexBuffer;

    // CPU-side data
    std::vector<Vertex> m_Vertices;
    std::vector<Tri> m_Tris;

    // Capacity and tracking
    bool m_IsDynamic;
    int m_VertexCapacity;
    int m_TriCapacity;
    int m_VertexCount;
    int m_TriCount;

    // Direct access to mapped memory
    void* m_MappedVertexData;
    void* m_MappedIndexData;
};