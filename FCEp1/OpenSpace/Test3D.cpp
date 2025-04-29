#include "Test3D.h"
#include "Importer.h"
#include "NodeCamera.h"
#include "SceneGraph.h"
#include "SolarisRenderer.h"
#include "GraphNode.h"
#include "NodeEntity.h"
#include "NodeLight.h"

void Test3D::InitState() {

	auto imp = new Importer;

	m_Ent1 = imp->ImportEntity("content/meshes/test/cube1.fbx");
	m_Graph1 = new SceneGraph;
	m_Cam1 = m_Graph1->GetCamera();

	m_Graph1->AddNode((GraphNode*)m_Ent1);

	int b = 5;

	m_Renderer = new SolarisRenderer;

	m_Renderer->SetRenderGraph(m_Graph1);

	m_Cam1->SetPosition(glm::vec3(0,0, 5));

	auto l1 = new NodeLight;
	m_Graph1->AddLight(l1);
	l1->SetPosition(glm::vec3(-5,0, 5));
	l1->SetRange(50);

}


void Test3D::UpdateState(float dt) {

}
float ang = 30;

void Test3D::RenderState() {

	ang = ang + 0.05;
	if (ang > 360) ang = 0;
	m_Ent1->SetRotation(glm::vec3(0, ang, 0));
	m_Renderer->RenderScene();
//	::vec3(ang,ang,ang));
}


void Test3D::EndState() {



}