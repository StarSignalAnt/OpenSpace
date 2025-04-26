#pragma once
#include "GameObj.h"

class FFile;

class GameLight : public GameObj
{
public:

	GameLight() : m_Diffuse(glm::vec3(1, 1, 1)), m_Range(1512) {};

	void SetRange(float range);
	float GetRange();
	void SetDiffuse(glm::vec3 diffuse) {
		m_Diffuse = diffuse;
	}
	glm::vec3 GetDiffuse()
	{
		return m_Diffuse;
	}
	void Write(FFile* f);
	void Read(FFile* f);



private:

	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;
	float m_Range;

};

