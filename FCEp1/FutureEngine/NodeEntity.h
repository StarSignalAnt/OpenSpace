#pragma once
#include "GraphNode.h"
#include <vector>
#include "Mesh3DBuffer.h"

class NodeEntity :
    public GraphNode
{
public:

    void AddMesh(Mesh3DBuffer* buffer);
    std::vector<Mesh3DBuffer*> GetMeshes();

private:

    std::vector<Mesh3DBuffer*> m_Meshes;

};

