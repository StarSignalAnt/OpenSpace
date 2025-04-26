#include "GameObj.h"
#include "MathsOps.h"

void GameObj::SetPosition(glm::vec3 position) {
	m_Position = position;
}

void GameObj::SetPosition(glm::vec2 position) {
	m_Position = glm::vec3(position, 1);
}

void GameObj::SetRotation(glm::vec3 rotation) {
	m_Rotation = rotation;
}

void GameObj::Turn(glm::vec3 rotation) {
	m_Rotation += rotation;
}

void GameObj::Move(glm::vec3 delta) {
	m_Position += delta;
}

void GameObj::Move(glm::vec2 delta) {
	m_Position += glm::vec3(delta, 0);
}

void GameObj::MoveLocal(glm::vec2 delta) {

	auto realMove = MathsOps::TransformCoord(delta, 360.0 - m_Rotation.y, 1.0f);

	realMove = realMove / m_Position.z;
	Move(realMove);



}

glm::vec2 GameObj::Transform(glm::vec2 delta) {

	auto realMove = MathsOps::TransformCoord(delta, 360.0 - m_Rotation.y, 1.0f);

	realMove = realMove / m_Position.z;

	return realMove;

}

void GameObj::Zoom(float delta) {

	m_Position.z = m_Position.z + delta * m_Position.z;

}
