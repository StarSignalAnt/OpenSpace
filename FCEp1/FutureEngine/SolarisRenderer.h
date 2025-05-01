#pragma once

class SceneGraph;
class NodeCamera;
class GraphNode;
class GPBasic3D;
class GP3D;
class GPDepth;
class Texture2D;
class CubeRenderer;

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

private:

	SceneGraph* m_RenderGraph = nullptr;
	NodeCamera* m_RenderCamera = nullptr;
	GPBasic3D* m_GPBasic;
	GP3D* m_GP3D;
	GPDepth* m_GPDepth;
	Texture2D* m_Tex;
	CubeRenderer* m_CR;

};

