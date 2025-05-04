#include "RenderMaterial.h"
#include "NodeCamera.h"
#include "NodeLight.h"
#include "GraphNode.h"
#include "RenderTarget2D.h"

void RenderMaterial::SetTexture(Texture2D* texture, int index) {

	m_Textures[index] = texture;

}

void RenderMaterial::SetCamera(NodeCamera* camera) {

	m_RenderCamera = camera;

}

void RenderMaterial::SetLight(NodeLight* light) {

	m_RenderLight = light;

}

void RenderMaterial::SetNode(GraphNode* node) {


	m_RenderNode = node;

}

void RenderMaterial::SetMesh(Mesh3DBuffer* buffer, int index) {

	m_RenderBuffers[index] = buffer;

}

void RenderMaterial::SetMatrix(glm::mat4 matrix, int index) {

	m_RenderMatrices[index] = matrix;

}

void RenderMaterial::SetBoneMatrix(glm::mat4 matrix, int index) {

	m_RenderBoneMatrices[index] = index;

}

void RenderMaterial::SetShadow(RenderTargetCube* cube)
{

	m_Shadow = cube;

}
