#include "SolarisRenderer.h"
#include "SceneGraph.h"
#include "GraphNode.h"
#include "NodeEntity.h"
#include "GPBasic3D.h"
#include "GP3D.h"
#include "GPS3D.h"
#include "GPDepth.h"
#include "Texture2D.h"
#include "CubeRenderer.h"
#include "NodeSkeletalEntity.h"
#include "GPSDepth.h"
#include "GPParticle.h"
#include "ParticleSystem.h"
#include "Mesh3DBuffer.h"


#include "RenderStateNotationLoader.h"
#include "BasicMath.hpp"
#include "FutureApp.h"
#include "RenderMaterial.h"
#include <glm/gtc/type_ptr.hpp>


glm::mat4 LookAt(glm::vec3 from, glm::vec3 target) {
	// Compute the forward (look) direction
	glm::vec3 forward = glm::normalize(target - from);

	// Define a world up vector (can be adjusted if needed)
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Check for parallel up and forward to avoid invalid cross product
	if (glm::abs(glm::dot(forward, up)) > 0.999f) {
		up = glm::vec3(0.0f, 0.0f, 1.0f); // Use an alternate up vector
	}

	// Compute the right and adjusted up vectors
	glm::vec3 right = glm::normalize(glm::cross(up, forward));
	glm::vec3 adjustedUp = glm::cross(forward, right);

	// Create rotation matrix (columns = right, up, forward)
	glm::mat4 rotation = glm::mat4(1.0f);
	rotation[0] = glm::vec4(right, 0.0f);
	rotation[1] = glm::vec4(adjustedUp, 0.0f);
	rotation[2] = glm::vec4(-forward, 0.0f); // Negative Z-forward (right-handed)
	// rotation[3] is the translation column (leave as identity here)

	return rotation;
}

Diligent::float4x4 GLMToDiligentMatrix3D7(glm::mat4 mat)
{
	// Convert GLM matrix to raw float pointer
	const float* matPtr = glm::value_ptr(mat);

	// Create a Diligent matrix from the data (column-major)
	Diligent::float4x4 diligentMat = Diligent::float4x4::MakeMatrix(matPtr);

	// Transpose it to match HLSL row-major expectation (optional depending on shader layout)
	diligentMat.Transpose();

	return diligentMat;
}


using namespace Diligent;

#include "GraphicsUtilities.h"
SolarisRenderer::SolarisRenderer() {
	m_GPParticle = new GPParticle;
	m_GPBasic = new GPBasic3D;
	m_GP3D = new GP3D;
	m_GPDepth = new GPDepth;
	m_GPS3D = new GPS3D;
	m_GPSDepth = new GPSDepth;
	
	m_Tex = new Texture2D("engine/maps/white.png");
	m_CR = new CubeRenderer;
	BufferDesc InstBuffDesc;
	InstBuffDesc.Name = "Instance data buffer";
	// Use default usage as this buffer will only be updated when grid size changes
	InstBuffDesc.Usage = USAGE_DEFAULT;
	InstBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
	InstBuffDesc.Size = sizeof(float4x4) * 10000;
	FutureApp::m_Inst->GetDevice()->CreateBuffer(InstBuffDesc, nullptr, &m_InstanceBuffer);
	




}

void SolarisRenderer::AddParticleSystem(ParticleSystem* ps)
{
	m_ParticleSystems.push_back(ps);
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
	
	for (auto ps : m_ParticleSystems) {

		RenderParticleSystem(ps);

	}

}

void SolarisRenderer::RenderSceneDepth() {

	RenderNodeDepth(m_RenderGraph->GetRoot());

}

void SolarisRenderer::RenderNodeDepth(GraphNode* node) {

	if (dynamic_cast<NodeEntity*>(node)) {


		auto entity = dynamic_cast<NodeEntity*>(node);

		for (auto mesh : entity->GetMeshes()) {


			auto lights = m_RenderGraph->GetLights();

			auto mat = mesh->GetDepthMaterial();


			mat->SetCamera(m_RenderCamera);
			mat->SetNode(node);
			//mat->SetLight(light);

			mat->SetMesh(mesh, 0);
			mat->SetMatrix(node->GetWorldMatrix(), 0);
			mat->SetMatrix(m_RenderCamera->GetWorldMatrix(), 1);
			mat->SetMatrix(m_RenderCamera->GetProjectionMatrix(), 2);
			//mat->SetShadow(light->GetShadowMap());
			mat->Bind();
			mat->Render();

			//for (auto light : lights) {
			//	m_Tex = mesh->GetMaterial()->GetColor();
			//	m_GPDepth->Bind(entity->GetWorldMatrix(), m_RenderCamera->GetWorldMatrix(), m_RenderCamera->GetProjectionMatrix(), m_RenderCamera, mesh);
			//	m_GPDepth->Render(mesh);


				int a = 5;
			//}


		}

	}
	else if(dynamic_cast<NodeSkeletalEntity*>(node))
	{

		auto entity = dynamic_cast<NodeSkeletalEntity*>(node);

		for (auto mesh : entity->GetMeshes()) {


			auto lights = m_RenderGraph->GetLights();

			auto mat = mesh->GetDepthMaterial();


			mat->SetCamera(m_RenderCamera);
			mat->SetNode(node);
			//mat->SetLight(light);

			mat->SetMesh(mesh, 0);
			mat->SetMatrix(node->GetWorldMatrix(), 0);
			mat->SetMatrix(m_RenderCamera->GetWorldMatrix(), 1);
			mat->SetMatrix(m_RenderCamera->GetProjectionMatrix(), 2);
			//mat->SetShadow(light->GetShadowMap());
			auto bones = entity->GetBones();
			for (int i = 0;i < 100;i++) {
				mat->SetBoneMatrix(bones[i], i);
			}
			mat->Bind();
			mat->Render();

			//for (auto light : lights) {
			//	m_Tex = mesh->GetMaterial()->GetColor();
			//	m_GPDepth->Bind(entity->GetWorldMatrix(), m_RenderCamera->GetWorldMatrix(), m_RenderCamera->GetProjectionMatrix(), m_RenderCamera, mesh);
			//	m_GPDepth->Render(mesh);


			int a = 5;
			//}


		}


	

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

			auto mat = mesh->GetMaterial();
			for (auto light : lights) {
				//m_Tex = mesh->GetMaterial()->GetColor();
				mat->SetCamera(m_RenderCamera);
				mat->SetNode(node);
				mat->SetLight(light);
				mat->SetMesh(mesh, 0);
				mat->SetMatrix(node->GetWorldMatrix(), 0);
				mat->SetMatrix(m_RenderCamera->GetWorldMatrix(), 1);
				mat->SetMatrix(m_RenderCamera->GetProjectionMatrix(), 2);
				mat->SetShadow(light->GetShadowMap());
				mat->Bind();
				mat->Render();

				//m_GP3D->Bind(entity->GetWorldMatrix(), m_RenderCamera->GetWorldMatrix(), m_RenderCamera->GetProjectionMatrix(),m_RenderCamera,light,true, mesh, m_Tex, mesh->GetMaterial()->GetNormal(),light->GetShadowMap());
				//m_GP3D->Render(mesh);

				int a = 5;
			}

		}

	}
	else if (dynamic_cast<NodeSkeletalEntity*>(node))
	{

		auto entity = dynamic_cast<NodeSkeletalEntity*>(node);

		for (auto mesh : entity->GetMeshes()) {



			auto lights = m_RenderGraph->GetLights();

			auto mat = mesh->GetMaterial();
			for (auto light : lights) {
				//m_Tex = mesh->GetMaterial()->GetColor();
				mat->SetCamera(m_RenderCamera);
				mat->SetNode(node);
				mat->SetLight(light);
				mat->SetMesh(mesh, 0);
				mat->SetMatrix(node->GetWorldMatrix(), 0);
				mat->SetMatrix(m_RenderCamera->GetWorldMatrix(), 1);
				mat->SetMatrix(m_RenderCamera->GetProjectionMatrix(), 2);
				mat->SetShadow(light->GetShadowMap());
				auto bones = entity->GetBones();
				for (int i = 0;i < 100;i++) {
					mat->SetBoneMatrix(bones[i], i);
				}
				mat->Bind();
				mat->Render();

				//m_GP3D->Bind(entity->GetWorldMatrix(), m_RenderCamera->GetWorldMatrix(), m_RenderCamera->GetProjectionMatrix(),m_RenderCamera,light,true, mesh, m_Tex, mesh->GetMaterial()->GetNormal(),light->GetShadowMap());
				//m_GP3D->Render(mesh);

				int a = 5;
			}

		}






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

void SolarisRenderer::RenderParticleSystem(ParticleSystem* ps)
{

	for (auto list : ps->GetLists()) {

		std::vector<PVertex> verts;

		//for (int i = 0;i < list->m_Particles.size();i++) {

			PVertex v1, v2, v3, v4;
			v1.position = glm::vec3(-1,-1,0 );
			v2.position = glm::vec3(1, -1, 0);
			v3.position = glm::vec3(1, 1, 0);
			v4.position = glm::vec3(-1, 1, 0);
			v1.Color = glm::vec4(1, 1, 1, 1);
			v2.Color = v1.Color;
			v3.Color = v1.Color;
			v4.Color = v1.Color;
			v1.UV = glm::vec3(0, 0, 0);
			v2.UV = glm::vec3(1, 0, 0);
			v3.UV = glm::vec3(1, 1, 0);
			v4.UV = glm::vec3(0, 1, 0);
			verts.push_back(v1);
			verts.push_back(v2);
			verts.push_back(v3);
			verts.push_back(v4);

		//}



		std::vector<Tri3D> indices;

		//int x = 0;

		//for (int i = 0;i 
		// < list->m_Particles.size();i++) {

		int x = 0;

			Tri3D t1;
			t1.indices.x = x;
			t1.indices.y = x + 1;
			t1.indices.z = x + 2;

			Tri3D t2;
			t2.indices.x = x + 2;
			t2.indices.y = x + 3;
			t2.indices.z = x;

			//x += 4;
			indices.push_back(t1);
			indices.push_back(t2);

		//}

		size_t requiredVertices = 4; // 4 vertices per quad
		size_t requiredIndices = 2; // 2 triangles per quad

		bool needNewVB = !VertexBuffer || m_VertexCapacity < requiredVertices;

		if (needNewVB)
		{
			// Wait until the GPU is done using the buffer, if needed (depends on your engine context handling)
			m_VertexCapacity = std::max(requiredVertices, m_VertexCapacity == 0 ? 256 : m_VertexCapacity * 2); // Avoid 0*2

			BufferDesc VertBuffDesc;
			VertBuffDesc.Name = "Dynamic Vertex Buffer";
			VertBuffDesc.Usage = USAGE_DYNAMIC;
			VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
			VertBuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
			VertBuffDesc.Size = static_cast<Uint32>(sizeof(PVertex) * m_VertexCapacity);

			RefCntAutoPtr<IBuffer> newVB;
			FutureApp::m_Inst->GetDevice()->CreateBuffer(VertBuffDesc, nullptr, &newVB);

			VertexBuffer = std::move(newVB); // Previous buffer is auto-released safely here
		}

		// === Index buffer ===
	//	size_t requiredIndices = indices.size() * 3; // each Tri3D has 3 indices
		bool needNewIB = !IndexBuffer || m_IndexCapacity < requiredIndices;

		if (needNewIB)
		{
			m_IndexCapacity = std::max(requiredIndices, m_IndexCapacity == 0 ? 256 : m_IndexCapacity * 2);

			BufferDesc IndBuffDesc;
			IndBuffDesc.Name = "Dynamic Index Buffer";
			IndBuffDesc.Usage = USAGE_DYNAMIC;
			IndBuffDesc.BindFlags = BIND_INDEX_BUFFER;
			IndBuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
			IndBuffDesc.Size = static_cast<Uint32>(sizeof(Uint32) * m_IndexCapacity); // Assuming Uint32 indices

			RefCntAutoPtr<IBuffer> newIB;
			FutureApp::m_Inst->GetDevice()->CreateBuffer(IndBuffDesc, nullptr, &newIB);

			IndexBuffer = std::move(newIB); // Safely replaces the old buffer
		}
	
		//const auto            zGridSize = static_cast<size_t>(m_GridSize);
		std::vector<float4x4> InstanceData(list->m_Particles.size());

		for (int i = 0;i < list->m_Particles.size();i++) {


			float scale = list->m_Particles[i]->size;
			auto mat = GLMToDiligentMatrix3D7(m_RenderCamera->GetProjectionMatrix() * m_RenderCamera->GetWorldMatrix() * (glm::translate(glm::mat4(1.0f), list->m_Particles[i]->position) * LookAt(list->m_Particles[i]->position, m_RenderCamera->GetPosition())*glm::scale(glm::mat4(1.0f),glm::vec3(scale,scale,scale))));


			InstanceData[i] = mat;

		}

		{
			MapHelper<PVertex> mappedVerts(FutureApp::m_Inst->GetContext(), VertexBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
			memcpy(mappedVerts, verts.data(), sizeof(PVertex) * verts.size());
		}

		// === Upload index data ===
		{
			MapHelper<Tri3D> mappedIndices(FutureApp::m_Inst->GetContext(), IndexBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
			memcpy(mappedIndices, indices.data(), sizeof(Tri3D) * indices.size());
		}

		// Update instance data buffer
		Uint32 DataSize = static_cast<Uint32>(sizeof(InstanceData[0]) * InstanceData.size());
		FutureApp::m_Inst->GetContext()->UpdateBuffer(m_InstanceBuffer, 0, DataSize, InstanceData.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		m_GPParticle->Bind(VertexBuffer, IndexBuffer, m_InstanceBuffer, list->m_Texture);

		DrawIndexedAttribs DrawAttrs;       // This is an indexed draw call
		DrawAttrs.IndexType = VT_UINT32; // Index type
		DrawAttrs.NumIndices = 6;
		DrawAttrs.NumInstances = list->m_Particles.size();
		// Verify the state of vertex and index buffers
		DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;
		FutureApp::m_Inst->GetContext()->DrawIndexed(DrawAttrs);

	}

}