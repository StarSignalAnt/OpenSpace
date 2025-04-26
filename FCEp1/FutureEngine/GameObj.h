#pragma once
#include <glm/glm.hpp>
class GameObj
{

public:
	virtual ~GameObj() = default;
	void SetPosition(glm::vec3 position);
	void SetRotation(glm::vec3 rotation);
	void SetPosition(glm::vec2 position);
	glm::vec3 GetPosition() const { return m_Position; }
	glm::vec3 GetRotation() const { return m_Rotation; }
	void Turn(glm::vec3 rotation);
	void Move(glm::vec3 delta);
	void Move(glm::vec2 delta);
	void MoveLocal(glm::vec2 delta);
	glm::vec2 Transform(glm::vec2 delta);
	void Zoom(float delta);


private:

protected:
	glm::vec3 m_Position;
	glm::vec3 m_Rotation;

};

