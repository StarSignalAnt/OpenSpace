#pragma once
#include "GraphNode.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>
#include <vector>
#include <string>
#include "BasicMath.hpp"
#include "BoneInfo.h"
#include "Mesh3DBuffer.h"
using namespace Diligent;

class Animator;
class Bone;

class NodeSkeletalEntity : public GraphNode
{
public:

	NodeSkeletalEntity() {
	//	m_Static = false;
	//	m_RTEnable = true
			;
		m_NodeName = "Actor";
		for (int i = 0; i < 100; i++) {

			m_Bones.push_back(float4x4::Identity());

		}
	};

	Mesh3DBuffer* ProcessMesh(aiMesh* mesh, aiScene* scene, bool cre_buf = true);
	void ExtractBoneWeights(Mesh3DBuffer* mesh, aiMesh* amesh, aiScene* scene);
	Vertex3D SetVertexBoneData(Vertex3D v, int boneID, float weight);
	std::map<std::string, BoneInfo*> GetBoneInfoMap() {
		return m_BoneInfoMap;
	}
	int GetBoneCount() {
		return m_BoneCounter;
	}
	void SetBoneCount(int bones) {
		m_BoneCounter = bones;
	}
	void SetBoneInfoMap(std::map<std::string, BoneInfo*> info)
	{
		m_BoneInfoMap = info;
	}
	void SetAnimator(Animator* animator) {
		m_Animator = animator;
	}

	void Render(bool sp);
	void Update(float delta) override;
	std::vector<float4x4> GetBones();
	Animator* GetAnimator() {
		return m_Animator;
	}
	float dt = 0.0f;
	void SetAnimTime(float time)
	{
		m_AnimTime = 0;
	}



	void UpdateAnim(float delta);
	Bone* FindBone(std::string name);
	std::vector<Mesh3DBuffer*> GetMeshes() {
		return m_Meshes;
	}
	
private:
	Animator* m_Animator = nullptr;
	std::map<std::string, BoneInfo*> m_BoneInfoMap;
	std::vector<float4x4> m_Bones;
	int m_BoneCounter = 0;
	float m_AnimTime = 0.0;
	std::vector<Mesh3DBuffer*> m_Meshes;
};

