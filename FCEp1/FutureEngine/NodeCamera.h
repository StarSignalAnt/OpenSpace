#pragma once
#include "GraphNode.h"
class NodeCamera :
    public GraphNode
{
public:

    glm::mat4 GetProjectionMatrix();
    glm::mat4 GetWorldMatrix() override;

private:

    float m_NearZ = 0.05f;
    float m_FarZ = 150.0f;
    float m_FOV = 45.0f;

};

