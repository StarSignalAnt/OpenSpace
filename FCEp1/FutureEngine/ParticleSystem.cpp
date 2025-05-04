#include "ParticleSystem.h"
#include "MathsOps.h"

ParticleSystem::ParticleSystem() {

}

void ParticleSystem::AddBase(Particle* particle) {

	m_BaseParticles.push_back(particle);

}

void ParticleSystem::Spawn(glm::vec3 position, int count)
{

	for (int i = 0;i < count;i++) {

		int num = MathsOps::RndInt(0, m_BaseParticles.size()-1);
		
		auto base = m_BaseParticles[num];

		Particle* particle = new Particle;
		particle->m_Texture = base->m_Texture;
		particle->position = position;
		particle->alpha = 1.0f;
		particle->alphaInertia = 1.0f;
		particle->color = glm::vec4(1, 1, 1, 1);
		particle->drag = glm::vec4(1, 1, 1, 1);
		particle->inertia = glm::vec3(1, 1, 1);
		particle->inertia = glm::vec3(MathsOps::RndFloat(-0.05, 0.05), MathsOps::RndFloat(-0.05, 0.05), MathsOps::RndFloat(-0.05, 0.05));
		//m_Particles.push_back(particle);
		auto list = GetList(base->m_Texture);
		list->m_Particles.push_back(particle);

	}

}

ParticleList* ParticleSystem::GetList(Texture2D* tex)
{

	for (auto list : m_ParticleLists) {
		if (list->m_Texture == tex) {
			return list;
		}
	}

	ParticleList* list = new ParticleList;
	list->m_Texture = tex;
	m_ParticleLists.push_back(list);
	return list;


}

void ParticleSystem::UpdateSystem() {

	for (auto list : m_ParticleLists) {

		for (auto p : list->m_Particles) {

			p->position += p->inertia;

		}

	}

}