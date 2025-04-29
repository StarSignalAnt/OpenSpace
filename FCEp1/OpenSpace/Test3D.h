#pragma once
#include <FutureState.h>

class NodeEntity;
class SceneGraph;
class NodeCamera;
class SolarisRenderer;

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

};

