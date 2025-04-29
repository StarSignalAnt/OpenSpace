#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

class GraphNode
{
public:

	GraphNode();
	void SetPosition(glm::vec3 position);
	void SetScale(glm::vec3 scale);
	void SetRotation(glm::vec3 rotation);
	virtual glm::mat4 GetWorldMatrix();
	void AddNode(GraphNode* node);
	std::vector<GraphNode*> GetNodes();
	void SetRootNode(GraphNode* node);
	GraphNode* GetRootNode();
	void SetName(std::string name);
	std::string GetName();
	glm::vec3 GetPosition();


protected:

	GraphNode* m_RootNode = nullptr;
	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::mat4 m_Rotation;
	std::vector<GraphNode*> m_Nodes;
	std::string m_NodeName = "Graph Node0";

};

