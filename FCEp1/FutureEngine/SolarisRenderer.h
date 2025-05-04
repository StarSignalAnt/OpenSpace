#pragma once

#include <vector>

#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
class SceneGraph;
class NodeCamera;
class GraphNode;
class GPBasic3D;
class GP3D;
class GPS3D;
class GPSDepth;
class GPDepth;
class GPParticle;
class Texture2D;
class CubeRenderer;
class ParticleSystem;

using namespace Diligent;

class SolarisRenderer
{
public:

	SolarisRenderer();

	void SetRenderGraph(SceneGraph* graph);
	void RenderSceneBasic();
	void RenderScene();
	void RenderSceneDepth();
	void RenderNodeDepth(GraphNode* node);
	void RenderNodeBasic(GraphNode* node);
	void RenderNode(GraphNode* node);
	void SetCamera(NodeCamera* cam);
	NodeCamera* GetCamera();
	void RenderShadowMaps();
	void AddParticleSystem(ParticleSystem* ps);
	void RenderParticleSystem(ParticleSystem* ps);

private:

	SceneGraph* m_RenderGraph = nullptr;
	NodeCamera* m_RenderCamera = nullptr;
	GPBasic3D* m_GPBasic;
	GP3D* m_GP3D;
	GPS3D* m_GPS3D;
	GPSDepth* m_GPSDepth;
	GPParticle* m_GPParticle;
	GPDepth* m_GPDepth;
	Texture2D* m_Tex;
	CubeRenderer* m_CR;
	RefCntAutoPtr<IBuffer>                m_InstanceBuffer;
	std::vector<ParticleSystem*> m_ParticleSystems;
	size_t m_VertexCapacity = 0;
	size_t m_IndexCapacity = 0;
	RefCntAutoPtr<IBuffer> VertexBuffer;
	RefCntAutoPtr<IBuffer> IndexBuffer;
};

