#pragma once
#include <FutureState.h>

class NodeEntity;
class SceneGraph;
class NodeCamera;
class SolarisRenderer;
class NodeFPSCamera;
class NodeLight;
class CubeRenderer;

class Test3D :
    public FutureState
{
public:


	void InitState() override;
	void UpdateState(float delta) override;
	void RenderState() override;
	void EndState() override;

private:

	NodeEntity* m_Ent1;
	SceneGraph* m_Graph1;
	NodeCamera* m_Cam1;
	SolarisRenderer* m_Renderer;
	NodeFPSCamera* m_FPSCam;
	NodeLight* m_Light1;
	CubeRenderer* m_CR;

};

