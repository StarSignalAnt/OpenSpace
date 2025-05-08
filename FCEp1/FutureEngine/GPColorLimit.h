#pragma once
#include "GraphicsPipeline.h"
class GPColorLimit :
    public GraphicsPipeline
{
public:

	GPColorLimit();
	void CreatePL() override;
	void Bind() override;
	void Bind(MeshBuffer* buffer, Texture2D* texture) override;
	void Render() override;
	void Render(MeshBuffer* buffer) override;

};

