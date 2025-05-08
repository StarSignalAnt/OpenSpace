#include "PPBloom.h"
#include "FutureApp.h"
#include "RenderTarget2D.h"
#include "GPColorLimit.h"
#include "GPBlur.h"
#include "GPCombine.h"

PPBloom::PPBloom() {

	int w = FutureApp::m_Inst->GetWidth();
	int h = FutureApp::m_Inst->GetHeight();

	CreateTarget(w, h);
	CreateTarget(w, h);

	InitProcess();
	
	m_CL = new GPColorLimit;
	m_Blur = new GPBlur;
	m_Combine = new GPCombine;

	int a = 5;

}

RenderTarget2D* PPBloom::Process(RenderTarget2D* input) {


	BindTarget(0);
	Draw(input,(GraphicsPipeline*) m_CL);
	ReleaseTarget(0);

	BindTarget(1);
	Draw(GetTarget(0),(GraphicsPipeline*)m_Blur);
	ReleaseTarget(1);

	BindTarget(0);
	Draw2(GetTarget(1),input->GetTexture(), (GraphicsPipeline*)m_Combine);
	ReleaseTarget(0);

	Draw(GetTarget(0));


	return nullptr;

}