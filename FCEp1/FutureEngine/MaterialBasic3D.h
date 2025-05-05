#pragma once
#include "RenderMaterial.h"
class MaterialBasic3D :
    public RenderMaterial
{
public:

    MaterialBasic3D();
    MaterialBasic3D(bool nothing);

    void InitPipeline() override;
    void Bind() override;
    void Render() override;
    RenderMaterial* Clone() override;


};

