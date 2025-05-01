#pragma once
#include "GraphNode.h"
#include "RenderTargetCube.h"

class NodeLight :
    public GraphNode
{
public:

    NodeLight() {
        m_DiffuseColor = glm::vec3(1, 1, 1);
        m_Shadow = new RenderTargetCube(2048);
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
    RenderTargetCube* GetShadowMap() {
        return m_Shadow;
    }

private:

    glm::vec3 m_DiffuseColor;
    float m_Range = 50;
    RenderTargetCube* m_Shadow;
};

