#pragma once
#include "RenderMaterial.h"
class MaterialBasic3D :
    public RenderMaterial
{
public:

    MaterialBasic3D();

    void InitPipeline() override;
    void Bind() override;
    void Render() override;



};

