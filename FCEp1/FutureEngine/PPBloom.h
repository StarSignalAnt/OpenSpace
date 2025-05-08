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

private:

    GPBlur* m_Blur;
    GPColorLimit* m_CL;
    GPCombine* m_Combine;

};


