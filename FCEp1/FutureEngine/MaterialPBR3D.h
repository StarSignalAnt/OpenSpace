#pragma once
#include "RenderMaterial.h"
class MaterialPBR3D :
    public RenderMaterial
{
public:


    MaterialPBR3D();

    void InitPipeline() override;
    void Bind() override;
    void Render() override;

};

