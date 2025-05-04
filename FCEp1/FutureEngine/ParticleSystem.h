#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Texture2D;

struct Particle {

	glm::vec3 position;
	glm::vec3 inertia;
	glm::vec3 drag;
	glm::vec4 color;
	float size = 0.1;
	float sizeInertia = 1.0;
	float rotation = 0;
	float rotationInertia;
	float alpha = 0.0f;
	float alphaInertia;
	Texture2D* m_Texture;

};

struct PVertex {

	glm::vec3 position;
	glm::vec3 UV;
	glm::vec4 Color;

};

class ParticleList {

public:
	std::vector<Particle*> m_Particles;
	Texture2D* m_Texture;

};

class ParticleSystem
{
public:

	ParticleSystem();
	void AddBase(Particle* particle);
	void Spawn(glm::vec3 position,int count);
	ParticleList* GetList(Texture2D* texture);
	std::vector<ParticleList*> GetLists()
	{
		return m_ParticleLists;
	}
	void UpdateSystem();

private:

	std::vector<ParticleList*> m_ParticleLists;
	std::vector<Particle*> m_BaseParticles;

	//std::vector<Particle*> m_Particles;

};

