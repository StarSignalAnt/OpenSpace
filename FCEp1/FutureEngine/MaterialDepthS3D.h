#pragma once
#include "RenderMaterial.h"

class MaterialDepthS3D : public RenderMaterial
{
public:

    MaterialDepthS3D();
    MaterialDepthS3D(bool nothing);

    void InitPipeline() override;
    void Bind() override;
    void Render() override;
    RenderMaterial* Clone() override;
};

