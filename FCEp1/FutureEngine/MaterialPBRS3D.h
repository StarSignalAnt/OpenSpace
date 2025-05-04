#pragma once
#include "RenderMaterial.h"
class MaterialPBRS3D :
    public RenderMaterial
{
public:

    MaterialPBRS3D();
    void InitPipeline() override;
    void Bind() override;
    void Render() override;
   

};

