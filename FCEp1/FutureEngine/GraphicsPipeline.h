#pragma once

#include <string>
#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "RenderStateNotationLoader.h"
#include "BasicMath.hpp"

class MeshBuffer;

using namespace Diligent;

class Texture2D;


class GraphicsPipeline
{
public:

	GraphicsPipeline(std::string vsh,std::string psh);
	GraphicsPipeline() {};
	virtual void CreatePL() = 0;
	virtual void Bind() = 0;
	virtual void Render() = 0;
	virtual void Render(MeshBuffer* buffer) = 0;
	virtual void Bind(MeshBuffer* buffer, Texture2D* texture) = 0;
	void SetWorld(float4x4 world);
	void SetAux(Texture2D* texture) {
		m_AuxTexture = texture;
	}
	void SetOther(Texture2D* other) {
		m_OtherTexture = other;
	}

protected:

	float4x4 m_World;
	RefCntAutoPtr<IPipelineState> m_pPSO;
	std::string m_VSPath;
	std::string m_PSPath;
	Texture2D* m_AuxTexture = nullptr;
	Texture2D* m_OtherTexture = nullptr;
	RefCntAutoPtr<IBuffer> m_Constants;
	RefCntAutoPtr<IShaderResourceBinding> m_SRB;
};

