#include "MeshBuffer.h"
#include "FutureApp.h"
#include <algorithm>

MeshBuffer::MeshBuffer() : m_IsDynamic(false), m_VertexCapacity(0), m_TriCapacity(0),
m_VertexCount(0), m_TriCount(0), m_MappedVertexData(nullptr), m_MappedIndexData(nullptr) {
    // Optimized empty constructor
}

MeshBuffer::MeshBuffer(int vertexCapacity, int triangleCapacity) :
    m_IsDynamic(true), m_VertexCapacity(vertexCapacity), m_TriCapacity(triangleCapacity),
    m_VertexCount(0), m_TriCount(0), m_MappedVertexData(nullptr), m_MappedIndexData(nullptr) {

    // Pre-allocate vectors with exact capacity (no reallocation)
    m_Vertices.reserve(vertexCapacity);
    m_Tris.reserve(triangleCapacity);

    // Create pre-sized GPU buffers
    CreateDynamicBuffers(vertexCapacity, triangleCapacity);
}

void MeshBuffer::CreateDynamicBuffers(int vertexCapacity, int triangleCapacity) {
    // Create vertex buffer
    BufferDesc VertBuffDesc;
    VertBuffDesc.Name = "Dynamic vertex buffer";
    VertBuffDesc.Usage = USAGE_DYNAMIC;
    VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
    VertBuffDesc.Size = vertexCapacity * sizeof(Vertex);
    VertBuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
    FutureApp::m_Inst->GetDevice()->CreateBuffer(VertBuffDesc, nullptr, &m_VertexBuffer);

    // Create index buffer
    BufferDesc IndBuffDesc;
    IndBuffDesc.Name = "Dynamic index buffer";
    IndBuffDesc.Usage = USAGE_DYNAMIC;
    IndBuffDesc.BindFlags = BIND_INDEX_BUFFER;
    IndBuffDesc.Size = triangleCapacity * sizeof(Tri);
    IndBuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
    FutureApp::m_Inst->GetDevice()->CreateBuffer(IndBuffDesc, nullptr, &m_IndexBuffer);
}

void MeshBuffer::BeginUpdate() {
    if (!m_IsDynamic || !m_VertexBuffer || !m_IndexBuffer)
        return;

    // Map both buffers at once to reduce API calls
    FutureApp::m_Inst->GetContext()->MapBuffer(m_VertexBuffer, MAP_WRITE, MAP_FLAG_DISCARD, m_MappedVertexData);
    FutureApp::m_Inst->GetContext()->MapBuffer(m_IndexBuffer, MAP_WRITE, MAP_FLAG_DISCARD, m_MappedIndexData);
}

void MeshBuffer::EndUpdate() {
    if (!m_IsDynamic || !m_VertexBuffer || !m_IndexBuffer)
        return;

    if (m_MappedVertexData) {
        FutureApp::m_Inst->GetContext()->UnmapBuffer(m_VertexBuffer, MAP_WRITE);
        m_MappedVertexData = nullptr;
    }

    if (m_MappedIndexData) {
        FutureApp::m_Inst->GetContext()->UnmapBuffer(m_IndexBuffer, MAP_WRITE);
        m_MappedIndexData = nullptr;
    }
}

void MeshBuffer::AddVertex(Vertex vertex) {
    // Check if there's room for more vertices
    if (m_VertexCount >= m_VertexCapacity && m_IsDynamic) {
        // Double capacity for future growth
        const int newCapacity = std::max(16, m_VertexCapacity * 2);
        ResizeVertexBuffer(newCapacity);
    }

    // Add vertex to local array
    if (m_VertexCount < m_Vertices.capacity()) {
        if (m_VertexCount >= m_Vertices.size())
            m_Vertices.push_back(vertex);
        else
            m_Vertices[m_VertexCount] = vertex;

        // Direct GPU update if buffer is mapped
        if (m_MappedVertexData) {
            static_cast<Vertex*>(m_MappedVertexData)[m_VertexCount] = vertex;
        }

        ++m_VertexCount;
    }
}

void MeshBuffer::AddTriangle(Tri triangle) {
    // Check if there's room for more triangles
    if (m_TriCount >= m_TriCapacity && m_IsDynamic) {
        // Double capacity for future growth
        const int newCapacity = std::max(16, m_TriCapacity * 2);
        ResizeIndexBuffer(newCapacity);
    }

    // Add triangle to local array
    if (m_TriCount < m_Tris.capacity()) {
        if (m_TriCount >= m_Tris.size())
            m_Tris.push_back(triangle);
        else
            m_Tris[m_TriCount] = triangle;

        // Direct GPU update if buffer is mapped
        if (m_MappedIndexData) {
            static_cast<Tri*>(m_MappedIndexData)[m_TriCount] = triangle;
        }

        ++m_TriCount;
    }
}

void MeshBuffer::ResizeVertexBuffer(int newCapacity) {
    // End any current mapping
    EndUpdate();

    // Create new buffer
    RefCntAutoPtr<IBuffer> newBuffer;
    BufferDesc VertBuffDesc;
    VertBuffDesc.Name = "Resized vertex buffer";
    VertBuffDesc.Usage = USAGE_DYNAMIC;
    VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
    VertBuffDesc.Size = newCapacity * sizeof(Vertex);
    VertBuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
    FutureApp::m_Inst->GetDevice()->CreateBuffer(VertBuffDesc, nullptr, &newBuffer);

    // Copy existing data to new buffer
    if (m_VertexCount > 0) {
        void* pData = nullptr;
        FutureApp::m_Inst->GetContext()->MapBuffer(newBuffer, MAP_WRITE, MAP_FLAG_DISCARD, pData);
        memcpy(pData, m_Vertices.data(), m_VertexCount * sizeof(Vertex));
        FutureApp::m_Inst->GetContext()->UnmapBuffer(newBuffer, MAP_WRITE);
    }

    // Update buffer and capacity
    m_VertexBuffer = newBuffer;
    m_VertexCapacity = newCapacity;
}

void MeshBuffer::ResizeIndexBuffer(int newCapacity) {
    // End any current mapping
    EndUpdate();

    // Create new buffer
    RefCntAutoPtr<IBuffer> newBuffer;
    BufferDesc IndBuffDesc;
    IndBuffDesc.Name = "Resized index buffer";
    IndBuffDesc.Usage = USAGE_DYNAMIC;
    IndBuffDesc.BindFlags = BIND_INDEX_BUFFER;
    IndBuffDesc.Size = newCapacity * sizeof(Tri);
    IndBuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
    FutureApp::m_Inst->GetDevice()->CreateBuffer(IndBuffDesc, nullptr, &newBuffer);

    // Copy existing data to new buffer
    if (m_TriCount > 0) {
        void* pData = nullptr;
        FutureApp::m_Inst->GetContext()->MapBuffer(newBuffer, MAP_WRITE, MAP_FLAG_DISCARD, pData);
        memcpy(pData, m_Tris.data(), m_TriCount * sizeof(Tri));
        FutureApp::m_Inst->GetContext()->UnmapBuffer(newBuffer, MAP_WRITE);
    }

    // Update buffer and capacity
    m_IndexBuffer = newBuffer;
    m_TriCapacity = newCapacity;
}

void MeshBuffer::Finalize() {
    // End any existing mappings
    EndUpdate();

    if (!m_VertexBuffer || !m_IndexBuffer) {
        // First-time creation
        if (m_IsDynamic) {
            // Dynamic buffers
            CreateDynamicBuffers(
                std::max(m_VertexCapacity, static_cast<int>(m_Vertices.size())),
                std::max(m_TriCapacity, static_cast<int>(m_Tris.size()))
            );

            // Update capacity tracking
            m_VertexCapacity = std::max(m_VertexCapacity, static_cast<int>(m_Vertices.size()));
            m_TriCapacity = std::max(m_TriCapacity, static_cast<int>(m_Tris.size()));

            // Copy data to GPU
            UpdateBuffers();
        }
        else {
            // Immutable buffers (original behavior)
            BufferDesc VertBuffDesc;
            VertBuffDesc.Name = "Immutable vertex buffer";
            VertBuffDesc.Usage = USAGE_IMMUTABLE;
            VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
            VertBuffDesc.Size = m_Vertices.size() * sizeof(Vertex);

            BufferData VBData;
            VBData.pData = m_Vertices.data();
            VBData.DataSize = m_Vertices.size() * sizeof(Vertex);
            FutureApp::m_Inst->GetDevice()->CreateBuffer(VertBuffDesc, &VBData, &m_VertexBuffer);

            BufferDesc IndBuffDesc;
            IndBuffDesc.Name = "Immutable index buffer";
            IndBuffDesc.Usage = USAGE_IMMUTABLE;
            IndBuffDesc.BindFlags = BIND_INDEX_BUFFER;
            IndBuffDesc.Size = m_Tris.size() * sizeof(Tri);

            BufferData IBData;
            IBData.pData = m_Tris.data();
            IBData.DataSize = m_Tris.size() * sizeof(Tri);
            FutureApp::m_Inst->GetDevice()->CreateBuffer(IndBuffDesc, &IBData, &m_IndexBuffer);

            // Update counters
            m_VertexCount = static_cast<int>(m_Vertices.size());
            m_TriCount = static_cast<int>(m_Tris.size());
        }
    }
    else if (m_IsDynamic) {
        // Update existing dynamic buffers
        UpdateBuffers();
    }
}

void MeshBuffer::UpdateBuffers() {
    // Skip if no data to update
    if (m_VertexCount == 0 && m_TriCount == 0)
        return;

    // Update vertex buffer
    if (m_VertexCount > 0) {
        void* pData = nullptr;
        FutureApp::m_Inst->GetContext()->MapBuffer(m_VertexBuffer, MAP_WRITE, MAP_FLAG_DISCARD, pData);
        // Use vector's internal array directly
        memcpy(pData, m_Vertices.data(), m_VertexCount * sizeof(Vertex));
        FutureApp::m_Inst->GetContext()->UnmapBuffer(m_VertexBuffer, MAP_WRITE);
    }

    // Update index buffer
    if (m_TriCount > 0) {
        void* pData = nullptr;
        FutureApp::m_Inst->GetContext()->MapBuffer(m_IndexBuffer, MAP_WRITE, MAP_FLAG_DISCARD, pData);
        memcpy(pData, m_Tris.data(), m_TriCount * sizeof(Tri));
        FutureApp::m_Inst->GetContext()->UnmapBuffer(m_IndexBuffer, MAP_WRITE);
    }
}

void MeshBuffer::Clear() {
    // Reset counters without deallocating memory
    m_VertexCount = 0;
    m_TriCount = 0;

    // We don't actually clear the vectors - just reset the logical count
    // This avoids deallocating memory that we'll likely use again
}