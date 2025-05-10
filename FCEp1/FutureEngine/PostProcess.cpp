#include "PostProcess.h"
#include "RenderTarget2D.h"
#include "SmartDraw.h"
#include "FutureApp.h"
#include "UIHelp.h"

void PostProcess::CreateTarget(int w, int h)
{

	auto target = new RenderTarget2D(w, h,false);
	m_Targets.push_back(target);

}

void PostProcess::CreateTargets(int num, int w, int h) {

	for (int i = 0;i < num;i++) {
		CreateTarget(w, h);
	}

}

void PostProcess::InitProcess() {

	m_Draw = new SmartDraw;

}

void PostProcess::BindTarget(int index,glm::vec3 col ) {

	m_Targets[index]->Bind();
	m_Targets[index]->Clear(col);
	m_Targets[index]->ClearZ();

}

void PostProcess::ReleaseTarget(int index) {

	m_Targets[index]->Unbind();

}

void PostProcess::Draw(RenderTarget2D* target,int w,int h)
{

	if (w == -1 || h == -1) {

		w = FutureApp::m_Inst->GetWidth();
		h = FutureApp::m_Inst->GetHeight();


	}


	UIHelp::RemoveScissor();
	UIHelp::Begin();

	UIHelp::DrawImage( glm::vec2(0, 0), glm::vec2(w, h), target->GetTexture(),glm::vec4(1,1,1,1));

	UIHelp::End();


}


void PostProcess::Draw(RenderTarget2D* target,GraphicsPipeline* pipe, int w, int h)
{

	if (w == -1 || h == -1) {

		w = FutureApp::m_Inst->GetWidth();
		h = FutureApp::m_Inst->GetHeight();


	}


	UIHelp::RemoveScissor();
	UIHelp::SetPipeline(pipe);
	UIHelp::Begin();


	UIHelp::DrawImage(glm::vec2(0, 0), glm::vec2(w, h), target->GetTexture(), glm::vec4(1, 1, 1, 1));

	UIHelp::End();
	UIHelp::DefaultPipeline();


}


void PostProcess::Draw2(RenderTarget2D* target,Texture2D* tex, GraphicsPipeline* pipe, int w, int h)
{

	if (w == -1 || h == -1) {

		w = FutureApp::m_Inst->GetWidth();
		h = FutureApp::m_Inst->GetHeight();


	}


	UIHelp::RemoveScissor();
	UIHelp::SetPipeline(pipe);
	UIHelp::Begin();


	UIHelp::DrawImage2(glm::vec2(0, 0), glm::vec2(w, h), target->GetTexture(),tex, glm::vec4(1, 1, 1, 1));

	UIHelp::End();
	UIHelp::DefaultPipeline();


}


RenderTarget2D* PostProcess::GetTarget(int index) {

	return m_Targets[index];

}