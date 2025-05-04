#include "Test3D.h"
#include "Importer.h"
#include "NodeCamera.h"
#include "SceneGraph.h"
#include "SolarisRenderer.h"
#include "GraphNode.h"
#include "NodeEntity.h"
#include "NodeLight.h"
#include "GameInput.h"
#include "NodeFPSCamera.h"
#include "CubeRenderer.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "NodeSkeletalEntity.h"
#include "ParticleSystem.h"
#include "Texture2D.h"

void Test3D::InitState() {

	auto imp = new Importer;

	NodeEntity* e2;
	//for (int i = 0;i < 100;i++) {
		m_Ent1 = imp->ImportEntity("content/meshes/test/cube1.fbx");
		e2 = imp->ImportEntity("content/meshes/test/cube2.fbx");
	
		m_S1 = imp->ImportSkeletal("test/test1.fbx");
	m_Graph1 = new SceneGraph;

	//m_Cam1 = m_Graph1->GetCamera();
	m_Cam1 = new NodeFPSCamera;

	m_Graph1->SetCamera(m_Cam1);

	m_Graph1->AddNode((GraphNode*)m_Ent1);
//	m_Graph1->AddNode((GraphNode*)e2);
	m_Graph1->AddNode((GraphNode*)m_S1);
	m_S1->SetScale(glm::vec3(0.02, 0.02, 0.02));
	int b = 5;

	m_Renderer = new SolarisRenderer;

	m_Renderer->SetRenderGraph(m_Graph1);

	m_Cam1->SetPosition(glm::vec3(0,3.5, 4));
	e2->SetPosition(glm::vec3(0, 3,-3));
	auto l1 = new NodeLight;
	m_Graph1->AddLight(l1);
	l1->SetPosition(glm::vec3(0,3.5,4));
	l1->SetRange(45);
	m_Light1 = l1;
	m_PS1 = new ParticleSystem;

	m_Renderer->AddParticleSystem(m_PS1);

	Particle* b_p1 = new Particle;
	b_p1->m_Texture = new Texture2D("content/sprites/stars/star2.png",true);

	m_PS1->AddBase(b_p1);



	m_CR = new CubeRenderer(512);

}

float ang = 30;
void Test3D::UpdateState(float dt) {

	auto delta = GameInput::MouseDelta;
	m_Graph1->UpdateScene(dt);
	if (GameInput::Keys[GLFW_KEY_SPACE])
	{
		m_Light1->SetPosition(m_Cam1->GetPosition());
	}
	ang = ang + 10*dt;
	if (ang > 360) ang = 0;
//	m_Ent1->SetRotation(glm::vec3(0, ang, 0));
}


void Test3D::RenderState() {

	//m_PS1->Spawn(glm::vec3(0, 3, 0), 15);
	
	//m_PS1->Spawn(glm::vec3(0, 3, 0), 5);
	//m_CR->RenderDepth(m_Renderer, m_Cam1->GetPosition(),50);
	m_PS1->UpdateSystem();
	m_Renderer->RenderShadowMaps();
	m_Renderer->RenderScene();
//	::vec3(ang,ang,ang));
}


void Test3D::EndState() {



}