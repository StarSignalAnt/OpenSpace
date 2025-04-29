#include "NodeEntity.h"

void NodeEntity::AddMesh(Mesh3DBuffer* buffer) {

	m_Meshes.push_back(buffer);

}

std::vector<Mesh3DBuffer*> NodeEntity::GetMeshes()
{

	return m_Meshes;

}