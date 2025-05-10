#pragma once
#include "PostProcess.h"

class RenderTarget2D;
class GPColorLimit;
class GPBlur;
class GPCombine;

class PPBloom :
    public PostProcess
{
public:

    PPBloom();
    RenderTarget2D* Process(RenderTarget2D* input) override;
    void SetBlurFactor(float blur);
    void SetCombineFactor(float f1, float f2);
private:

    GPBlur* m_Blur;
    GPColorLimit* m_CL;
    GPCombine* m_Combine;
    float m_BlurFactor = 0.1;
    float m_Combine1=0.5, m_Combine2 = 0.5;

};


