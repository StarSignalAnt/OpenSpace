#include "NodeFPSCamera.h"
#include "GameInput.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

void NodeFPSCamera::Update(float dt) {

	auto delta = GameInput::MouseDelta;

	m_Pitch -= delta.y * 0.2f;
	m_Yaw -= delta.x * 0.2f;

	SetRotation(glm::vec3(m_Pitch, m_Yaw, 0));

	float spd = 1.0f;
	if (GameInput::Keys[GLFW_KEY_LEFT_SHIFT])
	{
		spd = 2.0f;
	}

	if (GameInput::Keys[GLFW_KEY_W]) {

		Move(glm::vec3(0, 0, -2.5f * dt*spd));

	}
	if (GameInput::Keys[GLFW_KEY_A])
	{
		Move(glm::vec3(-2.5f*dt*spd, 0, 0));
	}
	if (GameInput::Keys[GLFW_KEY_S])
	{
		Move(glm::vec3(0, 0, 2.5 * dt*spd));
	}
	if (GameInput::Keys[GLFW_KEY_D])
	{
		Move(glm::vec3(2.5 * dt*spd, 0, 0));
	}



}