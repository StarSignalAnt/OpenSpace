#pragma once
#include "RenderMaterial.h"
class MaterialDepth3D :
    public RenderMaterial
{
public:


    MaterialDepth3D();

    void InitPipeline() override;
    void Bind() override;
    void Render() override;

};

