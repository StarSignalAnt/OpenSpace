#include "IntroState.h"
#include "FutureApp.h"
#include "SoundLib.h"
#include "SmartDraw.h"
#include "Texture2D.h"
#include "UIHelp.h"

void IntroState::InitState() {
	auto boot = FutureApp::m_Inst->SLib->loadSound("content/music/theme.wav");
	FutureApp::m_Inst->SLib->playSound(boot);

	m_Draw = new SmartDraw;
	m_Title = new Texture2D("content/images/titles/title.jpg");
}

void IntroState::UpdateState(float dt) {

}

void IntroState::RenderState() {

	int mx, my;

	mx = FutureApp::m_Inst->GetWidth() / 2;
	my = FutureApp::m_Inst->GetHeight() / 2;
	UIHelp::RemoveScissor();
	//m_Draw->Begin();
	//m_Draw->Draw(glm::vec2(mx, my), glm::vec2(mx * 2, my * 2),glm::vec4(1,1,1,1), m_Title,0,1);
//	m_Draw->End();
	UIHelp::Begin();
	UIHelp::DrawImage(glm::vec2(mx, my), glm::vec2(mx * 2, my * 2), m_Title, glm::vec4(1, 1, 1, 1), 0, 1.0f);
	UIHelp::End();

}

void IntroState::EndState() {

}