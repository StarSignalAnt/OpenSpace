#pragma once
#include "GraphNode.h"
#include "NodeCamera.h"

class NodeFPSCamera :
    public NodeCamera
{
public:

    void Update(float dt) override;

private:
    
    float m_Pitch=0, m_Yaw = 0;

};

