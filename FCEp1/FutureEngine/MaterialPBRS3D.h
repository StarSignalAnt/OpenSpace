#pragma once
#include "RenderMaterial.h"
class MaterialPBRS3D :
    public RenderMaterial
{
public:

    MaterialPBRS3D();
    MaterialPBRS3D(bool nothing);

    void InitPipeline() override;
    void Bind() override;
    void Render() override;
    RenderMaterial* Clone() override;

};

