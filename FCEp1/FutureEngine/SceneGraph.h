#pragma once
#include "GraphNode.h"
#include "NodeCamera.h"
#include "NodeLight.h"

class SceneGraph
{
public:

	SceneGraph();
	void SetCamera(NodeCamera* camera);
	NodeCamera* GetCamera();
	GraphNode* GetRoot();
	void AddNode(GraphNode* node);
	void AddLight(NodeLight* light);
	std::vector<NodeLight*> GetLights();
	void UpdateScene(float dt);

private:

	GraphNode* m_RootNode = nullptr;
	NodeCamera* m_Camera = nullptr;
	std::vector<NodeLight*> m_Lights;

};

