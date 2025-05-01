#include "SolarisRenderer.h"
#include "SceneGraph.h"
#include "GraphNode.h"
#include "NodeEntity.h"
#include "GPBasic3D.h"
#include "GP3D.h"
#include "GPDepth.h"
#include "Texture2D.h"
#include "CubeRenderer.h"

SolarisRenderer::SolarisRenderer() {

	m_GPBasic = new GPBasic3D;
	m_GP3D = new GP3D;
	m_GPDepth = new GPDepth;
	m_Tex = new Texture2D("engine/maps/white.png");
	m_CR = new CubeRenderer;


}

void SolarisRenderer::SetRenderGraph(SceneGraph* graph) {

	m_RenderGraph = graph;
	m_RenderCamera = graph->GetCamera();



}

void SolarisRenderer::RenderSceneBasic() {

	RenderNodeBasic(m_RenderGraph->GetRoot());

}

void SolarisRenderer::RenderScene() {
	RenderNode(m_RenderGraph->GetRoot());
}

void SolarisRenderer::RenderSceneDepth() {

	RenderNodeDepth(m_RenderGraph->GetRoot());

}

void SolarisRenderer::RenderNodeDepth(GraphNode* node) {

	if (dynamic_cast<NodeEntity*>(node)) {


		auto entity = dynamic_cast<NodeEntity*>(node);

		for (auto mesh : entity->GetMeshes()) {


			auto lights = m_RenderGraph->GetLights();


			//for (auto light : lights) {
			//	m_Tex = mesh->GetMaterial()->GetColor();
				m_GPDepth->Bind(entity->GetWorldMatrix(), m_RenderCamera->GetWorldMatrix(), m_RenderCamera->GetProjectionMatrix(), m_RenderCamera, mesh);
				m_GPDepth->Render(mesh);

				int a = 5;
			//}


		}

	}
	else {



	}

	for (auto sub : node->GetNodes()) {

		RenderNodeDepth(sub);

	}

}

void SolarisRenderer::RenderNodeBasic(GraphNode* node) {


	if (dynamic_cast<NodeEntity*>(node)) {


		auto entity = dynamic_cast<NodeEntity*>(node);

		for (auto mesh : entity->GetMeshes()) {

			m_Tex = mesh->GetMaterial()->GetColor();
			m_GPBasic->Bind(entity->GetWorldMatrix(), m_RenderCamera->GetWorldMatrix(), m_RenderCamera->GetProjectionMatrix(), mesh, m_Tex);
			m_GPBasic->Render(mesh);
			int a = 5;

		}

	}
	else {



	}

	for (auto sub : node->GetNodes()) {

		RenderNodeBasic(sub);

	}

}

void SolarisRenderer::RenderNode(GraphNode* node) {

	if (dynamic_cast<NodeEntity*>(node)) {


		auto entity = dynamic_cast<NodeEntity*>(node);

		for (auto mesh : entity->GetMeshes()) {


			auto lights = m_RenderGraph->GetLights();


			for (auto light : lights) {
				m_Tex = mesh->GetMaterial()->GetColor();
				m_GP3D->Bind(entity->GetWorldMatrix(), m_RenderCamera->GetWorldMatrix(), m_RenderCamera->GetProjectionMatrix(),m_RenderCamera,light,true, mesh, m_Tex, mesh->GetMaterial()->GetNormal(),light->GetShadowMap());
				m_GP3D->Render(mesh);
				int a = 5;
			}

		}

	}
	else {



	}

	for (auto sub : node->GetNodes()) {

		RenderNode(sub);

	}


}

NodeCamera* SolarisRenderer::GetCamera() {

	return m_RenderGraph->GetCamera();

}

void SolarisRenderer::SetCamera(NodeCamera* camera) {

	m_RenderGraph->SetCamera(camera);
	m_RenderCamera = camera;

}

void SolarisRenderer::RenderShadowMaps() {

	auto lights = m_RenderGraph->GetLights();
	for (auto light : lights) {

		m_CR->SetTarget(light->GetShadowMap());
		m_CR->RenderDepth(this, light->GetPosition(), light->GetRange());

	}


}