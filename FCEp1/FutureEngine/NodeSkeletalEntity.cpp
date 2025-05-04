#include "NodeSkeletalEntity.h"
#include "NodeSkeletalEntity.h"
#include "Mesh3DBuffer.h"
#include "BasicMath.hpp"
#include "Animator.h"
#include "Importer.h"
#include "Animation.h"
#include "Bone.h"
using namespace Diligent;




float3 atov(aiVector3D v)
{

	return float3(v.x, v.y, v.z);

}

Diligent::float4x4 atov(const aiMatrix4x4& assimpMat) {
	Diligent::float4x4 diligentMat;

	diligentMat.m00 = assimpMat.a1; diligentMat.m01 = assimpMat.a2; diligentMat.m02 = assimpMat.a3; diligentMat.m03 = assimpMat.a4;
	diligentMat.m10 = assimpMat.b1; diligentMat.m11 = assimpMat.b2; diligentMat.m12 = assimpMat.b3; diligentMat.m13 = assimpMat.b4;
	diligentMat.m20 = assimpMat.c1; diligentMat.m21 = assimpMat.c2; diligentMat.m22 = assimpMat.c3; diligentMat.m23 = assimpMat.c4;
	diligentMat.m30 = assimpMat.d1; diligentMat.m31 = assimpMat.d2; diligentMat.m32 = assimpMat.d3; diligentMat.m33 = assimpMat.d4;

	return diligentMat.Transpose();
}

Mesh3DBuffer* NodeSkeletalEntity::ProcessMesh(aiMesh* mesh, aiScene* scene, bool buf_cre)
{

	auto res = new Mesh3DBuffer;
	//res->SetOwner(this);

	for (int i = 0; i < mesh->mNumVertices; i++) {

		Vertex3D v;

		v.position = atov(mesh->mVertices[i]);
		v.normal = atov(mesh->mNormals[i]);
		v.tangent = atov(mesh->mTangents[i]);
		v.bitangent = atov(mesh->mBitangents[i]);
		v.uv = atov(mesh->mTextureCoords[0][i]);
		v.uv.y = 1.0f - v.uv.y;
		v.color = float4(1, 1, 1, 1);
		res->AddVertex(v, true);


	}

	ExtractBoneWeights(res, mesh, scene);

	m_Meshes.push_back(res);

	for (int i = 0; i < mesh->mNumFaces; i++) {
		Tri3D t;
		t.indices.x = mesh->mFaces[i].mIndices[0];
		t.indices.y = mesh->mFaces[i].mIndices[2];
		t.indices.z = mesh->mFaces[i].mIndices[1];

		res->AddTriangle(t);

		Tri3D t2;

		t2.indices.x = t.indices.x;
		t2.indices.y = t.indices.z;
		t2.indices.z = t.indices.y;


	}

	if (buf_cre) {
		res->Finalize();
	}

	return res;
}

void NodeSkeletalEntity::ExtractBoneWeights(Mesh3DBuffer* mesh, aiMesh* amesh, aiScene* scene) {

	for (int i = 0; i < amesh->mNumBones; i++) {

		int boneID = -1;
		std::string name = amesh->mBones[i]->mName.C_Str();
		if (m_BoneInfoMap.count(name) == 0) {

			BoneInfo* bone = new BoneInfo;
			bone->ID = m_BoneCounter;
			bone->Offset = atov(amesh->mBones[i]->mOffsetMatrix);
			m_BoneInfoMap[name] = bone;
			boneID = m_BoneCounter;
			m_BoneCounter++;

		}
		else {
			boneID = m_BoneInfoMap[name]->ID;
		}

		auto weights = amesh->mBones[i]->mWeights;
		int num_weights = amesh->mBones[i]->mNumWeights;

		for (int weightIndex = 0; weightIndex < num_weights; weightIndex++)
		{
			int vert_id = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			auto v = SetVertexBoneData(mesh->GetVertex(vert_id), i, weight);
			mesh->SetVertex(vert_id, v);
		}


	}


}

Vertex3D NodeSkeletalEntity::SetVertexBoneData(Vertex3D vertex, int boneID, float weight) {

	if (vertex.bone_ids.x < 0)
	{
		vertex.bone_ids.x = boneID;
		vertex.bone_weights.x = weight;
		return vertex;
	}
	if (vertex.bone_ids.y < 0)
	{
		vertex.bone_ids.y = boneID;
		vertex.bone_weights.y = weight;
		return vertex;
	}
	if (vertex.bone_ids.z < 0)
	{
		vertex.bone_ids.z = boneID;
		vertex.bone_weights.z = weight;
		return vertex;
	}
	if (vertex.bone_ids.w < 0)
	{
		vertex.bone_ids.w = boneID;
		vertex.bone_weights.w = weight;
		return vertex;
	}
	return vertex;
}

void NodeSkeletalEntity::Update(float delta) {



	UpdateAnim(delta);


	auto bones = m_Animator->GetBones();


	for (int i = 0; i < 100; i++) {

		auto cur = m_Bones[i];
		auto nxt = bones[i];

		auto dif = nxt - cur;

		m_Bones[i] += dif * 0.35f;

	}


	for (auto msh : m_Meshes) {
	//	auto ml = (MaterialActorLight*)msh->GetMaterial();
	//	ml->SetBones(bones);
//		auto md = (MaterialActorDepth*)msh->GetDepthMaterial();
	//	md->SetBones(bones);


	}

}

void NodeSkeletalEntity::Render(bool sp) {

	


}

std::vector<float4x4> NodeSkeletalEntity::GetBones() {
	return m_Bones;
	//	return m_Animator->GetBones();
}

void NodeSkeletalEntity::UpdateAnim(float delta) {

	m_Animator->UpdateAnimation(delta);

}

Bone* NodeSkeletalEntity::FindBone(std::string name) {

	auto bones = m_Animator->GetRealBones();
	for (auto b : bones) {


		if (b->GetName() == name) {
			return b;
		}
		//printf("Bone:");
		//printf(b->GetName().c_str());
		//printf("\n");


	}

	int b = 5;
	return nullptr;

}
