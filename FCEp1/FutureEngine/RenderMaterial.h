#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
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
class NodeCamera;
class NodeLight;
class GraphNode;
class Mesh3DBuffer;
class RenderTargetCube;

class RenderMaterial
{
public:

	void SetColor(Texture2D* texture)
	{
		m_Color = texture;
	}
	void SetNormal(Texture2D* normal)
	{
		m_Normal = normal;
	}
	Texture2D* GetColor() {
		return m_Color;
	}
	Texture2D* GetNormal() {
		return m_Normal;
	}

	virtual void InitPipeline() = 0;

	void SetTexture(Texture2D* texture, int index);
	void SetCamera(NodeCamera* camera);
	void SetLight(NodeLight* light);
	void SetNode(GraphNode* node);
	void SetMesh(Mesh3DBuffer* buffer, int index);
	void SetMatrix(glm::mat4 matrix, int index);
	void SetBoneMatrix(float4x4 matrix, int index);
	void SetShadow(RenderTargetCube* cube);

	virtual void Bind() = 0;
	virtual void Render() = 0;

protected:

	Texture2D* m_Color = nullptr;
	Texture2D* m_Normal = nullptr;
	Texture2D* m_Rough = nullptr;
	Texture2D* m_Metal = nullptr;
	Texture2D* m_Textures[64];
	NodeCamera* m_RenderCamera;
	NodeLight* m_RenderLight;
	GraphNode* m_RenderNode;
	Mesh3DBuffer* m_RenderBuffers[128];
	glm::mat4 m_RenderMatrices[256];
	float4x4 m_RenderBoneMatrices[256];
	RenderTargetCube* m_Shadow;

	RefCntAutoPtr<IPipelineState> m_pPSO;

	std::string m_VSPath;
	std::string m_PSPath;
	RefCntAutoPtr<IBuffer> m_Constants;
	RefCntAutoPtr<IShaderResourceBinding> m_SRB;
};

