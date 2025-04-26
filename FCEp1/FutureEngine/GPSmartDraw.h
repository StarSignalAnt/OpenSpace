#pragma once
#include "GraphicsPipeline.h"

class MeshBuffer;
class Texture2D;

class GPSmartDraw :
    public GraphicsPipeline
{
public:

    GPSmartDraw();
	void CreatePL() override;
	void Bind() override;
	void Bind(MeshBuffer* buffer, Texture2D* texture) override;
	void Render() override;
	void Render(MeshBuffer* buffer) override;




};

