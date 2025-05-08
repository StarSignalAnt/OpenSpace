#pragma once
#include "GraphicsPipeline.h"
class GPCombine :
    public GraphicsPipeline
{
public:

	GPCombine();
	void CreatePL() override;
	void Bind() override;
	void Bind(MeshBuffer* buffer, Texture2D* texture) override;
	void Render() override;
	void Render(MeshBuffer* buffer) override;

private:



};

