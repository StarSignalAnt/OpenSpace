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
	void SetCombine(float f1, float f2);

private:

	float m_Combine1, m_Combine2;

};

