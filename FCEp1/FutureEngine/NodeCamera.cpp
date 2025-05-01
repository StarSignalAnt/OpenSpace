#include "NodeCamera.h"
#include "FutureApp.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
glm::mat4 NodeCamera::GetProjectionMatrix() {

	float width = FutureApp::m_Inst->GetWidth();
	float height = FutureApp::m_Inst->GetHeight();


	auto fov = glm::radians(m_FOV);

	return glm::perspective(fov, width / height, m_NearZ, m_FarZ);

}

glm::mat4 NodeCamera::GetWorldMatrix() {

	return glm::inverse(GraphNode::GetWorldMatrix());

}

void NodeCamera::SetFOV(float fov)
{
	m_FOV = fov;
}

void NodeCamera::SetFarZ(float z)
{
	m_FarZ = z;
}