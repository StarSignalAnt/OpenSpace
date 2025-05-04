#pragma once
#include "GraphicsPipeline.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Mesh3DBuffer;
class Texture2D;
class NodeLight;
class NodeCamera;
class RenderTargetCube;

class GPS3D :
    public GraphicsPipeline
{
public:
	GPS3D();
	void CreatePL() override;
	void Bind() override {
	}
	void Bind(MeshBuffer* buf, Texture2D* tex) override
	{

	}
	void Bind(glm::mat4 world, glm::mat4 view, glm::mat4 proj, NodeCamera* cam, NodeLight* light, bool first_light, Mesh3DBuffer* buffer, Texture2D* texture, Texture2D* normal, RenderTargetCube* shadow,std::vector<float4x4> bones);
	void Render() override {

	}
	void Render(MeshBuffer* buf) {

	}
	void Render(Mesh3DBuffer* buffer);


private:

};

