#pragma once
#include <glm/glm.hpp>
class RenderTargetCube;
class SceneGraph;
class SolarisRenderer;

class CubeRenderer
{
public:

	CubeRenderer(int size);
	CubeRenderer();
	void SetTarget(RenderTargetCube* cube) {

		m_Target = cube;
	}
	void Render(SolarisRenderer* graph, glm::vec3 position,float range);
	void RenderDepth(SolarisRenderer* graph, glm::vec3 position, float range);
private:

	RenderTargetCube* m_Target;

};

