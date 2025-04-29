#include "SceneGraph.h"

SceneGraph::SceneGraph() {

	m_Camera = new NodeCamera;
	m_RootNode = new GraphNode;
	

}

NodeCamera* SceneGraph::GetCamera() {

	return m_Camera;

}

GraphNode* SceneGraph::GetRoot() {

	return m_RootNode;

}

void SceneGraph::AddNode(GraphNode* node) {

	m_RootNode->AddNode(node);

}

void SceneGraph::AddLight(NodeLight* light) {

	m_Lights.push_back(light);

}

std::vector<NodeLight*> SceneGraph::GetLights() {

	return m_Lights;

}