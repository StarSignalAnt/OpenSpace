#include "PPDOF.h"
#include "FutureApp.h"
#include "RenderTarget2D.h"
#include "GPColorLimit.h"
#include "GPBlur.h"
#include "GPCombine.h"
#include "SolarisRenderer.h"
#include <glm/glm.hpp>
#include "GPDOF.h"

PPDOF::PPDOF() {

	int w = FutureApp::m_Inst->GetWidth();
	int h = FutureApp::m_Inst->GetHeight();

	CreateTarget(w, h);
	CreateTarget(w, h);

	InitProcess();
//	m_Depth = new RenderTarget2D(w, h, false);
	m_DOF = new GPDOF;
	int a = 5;

}

RenderTarget2D* PPDOF::Process(RenderTarget2D* input) {

	BindTarget(0,glm::vec3(1,1,1));
	m_Renderer->RenderSceneDepth();
	ReleaseTarget(0);

	BindTarget(1);
	//BindTarget(0);
	Draw2(GetTarget(0), input->GetTexture(), (GraphicsPipeline*)m_DOF);
//	ReleaseTarget(0);

	ReleaseTarget(1);


	Draw(GetTarget(1));

	return GetTarget(0);

}

void PPDOF::SetRenderer(SolarisRenderer* renderer) {

	m_Renderer = renderer;

}