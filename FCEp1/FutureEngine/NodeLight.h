#pragma once
#include "GraphNode.h"
class NodeLight :
    public GraphNode
{
public:

    NodeLight() {
        m_DiffuseColor = glm::vec3(1, 1, 1);
    }
    void SetDiffuse(glm::vec3 diffuse);
    glm::vec3 GetDiffuse() {
        return m_DiffuseColor;
    }
    void SetRange(float range) {
        m_Range = range;
    }
    float GetRange() {
        return m_Range;
    }

private:

    glm::vec3 m_DiffuseColor;
    float m_Range = 50;

};

