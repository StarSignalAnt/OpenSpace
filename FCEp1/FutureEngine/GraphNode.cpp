#include "GraphNode.h"
#include <iostream>
#include <sstream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
int node_id = 0;

GraphNode::GraphNode() {

	std::ostringstream ss;
	ss << "Graph Node" << node_id++ << std::endl;
	m_NodeName = ss.str();
	m_Rotation = glm::mat4(1.0f);
	m_Scale = glm::vec3(1, 1, 1);
	
}

void GraphNode::SetPosition(glm::vec3 position) {

	m_Position = position;

}

void GraphNode::SetScale(glm::vec3 scale) {

	m_Scale = scale;

}

void GraphNode::SetRotation(glm::vec3 rotation) {
//	glm::quat q = glm::quat(glm::radians(rotation)); // Convert Euler to quaternion
//	m_Rotation = glm::toMat4(q);
	float pitch = glm::radians(rotation.x);
	float yaw = glm::radians(rotation.y);
	float roll = glm::radians(rotation.z);

	glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1, 0, 0));
	glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0, 1, 0));
	glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), roll, glm::vec3(0, 0, 1));

	// Combine in Z * Y * X order
	m_Rotation = rotZ * rotY * rotX;

}

void GraphNode::SetRotation(glm::mat4 rotation) {

	m_Rotation = rotation;

}


glm::mat4 GraphNode::GetWorldMatrix() {

	glm::mat4 root =  glm::mat4(1.0);
	if (m_RootNode != nullptr) {

		root = m_RootNode->GetWorldMatrix();

	}

	glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);

	glm::mat4 local = translation * m_Rotation * scale;

	return root * local;



}

void GraphNode::AddNode(GraphNode* node) {

	m_Nodes.push_back(node);
	node->SetRootNode(this);

}

std::vector<GraphNode*> GraphNode::GetNodes() {

	return m_Nodes;

}

void GraphNode::SetRootNode(GraphNode* node) {

	m_RootNode = node;

}

GraphNode* GraphNode::GetRootNode() {

	return m_RootNode;

}

void GraphNode::SetName(std::string name) {

	m_NodeName = name;

}

std::string GraphNode::GetName() {

	return m_NodeName;

}

glm::vec3 GraphNode::GetPosition() {

	return m_Position;

}

void GraphNode::UpdateChildren(float dt) {

	Update(dt);
	for (auto node : m_Nodes) {
		node->UpdateChildren(dt);

	}

}

glm::vec3 GraphNode::TransformVector(glm::vec3 delta) {

	return m_Rotation * glm::vec4(delta, 1.0);

}

void GraphNode::Move(glm::vec3 delta) {

	m_Position += TransformVector(delta);

}

void GraphNode::LookAt(glm::vec3 target, glm::vec3 up)
{

	glm::vec3 forward = glm::normalize(target - m_Position);

	// Compute right and corrected up
	glm::vec3 right = glm::normalize(glm::cross(forward, up));
	glm::vec3 upCorrected = glm::cross(right, forward);

	// Create a look-at matrix manually (camera looks along -Z in OpenGL)
	glm::mat4 rot(1.0f);

	rot[0][0] = right.x;
	rot[1][0] = right.y;
	rot[2][0] = right.z;

	rot[0][1] = upCorrected.x;
	rot[1][1] = upCorrected.y;
	rot[2][1] = upCorrected.z;

	rot[0][2] = -forward.x;
	rot[1][2] = -forward.y;
	rot[2][2] = -forward.z;

	rot[3][3] = 1.0f;

	m_Rotation = rot;

}