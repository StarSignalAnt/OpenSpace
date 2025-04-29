#pragma once

class SceneGraph;
class NodeCamera;
class GraphNode;
class GPBasic3D;
class GP3D;
class Texture2D;

class SolarisRenderer
{
public:

	SolarisRenderer() {

	}

	void SetRenderGraph(SceneGraph* graph);
	void RenderSceneBasic();
	void RenderScene();
	void RenderNodeBasic(GraphNode* node);
	void RenderNode(GraphNode* node);

private:

	SceneGraph* m_RenderGraph = nullptr;
	NodeCamera* m_RenderCamera = nullptr;
	GPBasic3D* m_GPBasic;
	GP3D* m_GP3D;
	Texture2D* m_Tex;

};

