#pragma once
#include "GraphicsPipeline.h"
class GPMap2D :
    public GraphicsPipeline
{
public:

	GPMap2D();
	void CreatePL() override;
	void Bind() override;
	void Bind(MeshBuffer* buffer, Texture2D* texture) override;
	void Render() override;
	void Render(MeshBuffer* buffer) override;

	float4 screenSize;
	float4 lightPos;
	float4 lightActual;
	float4 lightRange;
	float4 camRot;
	float4 camZoom;
	float4 lightDiffuse;
	float4 mapSize;
	float4 topY;
	Texture2D* m_Normal;

};

