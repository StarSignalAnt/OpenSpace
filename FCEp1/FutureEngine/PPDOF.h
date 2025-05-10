#pragma once
#include "PostProcess.h"


class GPDOF;
class m_Depth;
class SolarisRenderer;

class PPDOF :
    public PostProcess
{
public:

    PPDOF();
    void SetRenderer(SolarisRenderer* renderer);
    RenderTarget2D* Process(RenderTarget2D* input) override;


private:

    float m_TargetZ = 0.4f;
    float m_Range = 0.1;
    RenderTarget2D* m_Depth;
    SolarisRenderer* m_Renderer;
    GPDOF* m_DOF;

};

