#pragma once
#include "RenderMaterial.h"
class MaterialDepth3D :
    public RenderMaterial
{
public:


    MaterialDepth3D();
    MaterialDepth3D(bool nothing);

    void InitPipeline() override;
    void Bind() override;
    void Render() override;
    RenderMaterial* Clone() override;

};

