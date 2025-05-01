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
	void SetRotation(glm::mat4 rot);
	virtual glm::mat4 GetWorldMatrix();
	void AddNode(GraphNode* node);
	std::vector<GraphNode*> GetNodes();
	void SetRootNode(GraphNode* node);
	GraphNode* GetRootNode();
	void SetName(std::string name);
	std::string GetName();
	glm::vec3 GetPosition();
	virtual void Update(float dt) {};
	void UpdateChildren(float dt);
	void Move(glm::vec3 delta);
	glm::vec3 TransformVector(glm::vec3 delta);
	void LookAt(glm::vec3 target, glm::vec3 up);
protected:

	GraphNode* m_RootNode = nullptr;
	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::mat4 m_Rotation;
	std::vector<GraphNode*> m_Nodes;
	std::string m_NodeName = "Graph Node0";

};

