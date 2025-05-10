#pragma once
#include "GraphicsPipeline.h"
class GPBlur :
    public GraphicsPipeline
{
public:

	GPBlur();
	void CreatePL() override;
	void Bind() override;
	void Bind(MeshBuffer* buffer, Texture2D* texture) override;
	void Render() override;
	void Render(MeshBuffer* buffer) override;
	void SetBlurFactor(float blur);


	float m_BlurFactor = 0.1;


};

