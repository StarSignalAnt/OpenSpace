#pragma once
#include "RenderMaterial.h"

class MaterialDepthS3D : public RenderMaterial
{
public:

    MaterialDepthS3D();

    void InitPipeline() override;
    void Bind() override;
    void Render() override;

};

