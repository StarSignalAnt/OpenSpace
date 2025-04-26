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
	m_Images.push_back(new Texture2D("content/images/titles/starsignal.png"));

	m_Images.push_back(new Texture2D("content/images/titles/title.jpg"));
	m_Images.push_back(new Texture2D("content/images/titles/fight1.png"));
	m_Images.push_back(new Texture2D("content/images/titles/land1.png"));
	m_Images.push_back(new Texture2D("content/images/titles/land2.png"));
	m_Images.push_back(new Texture2D("content/images/titles/utopia1.png"));

}

void IntroState::UpdateState(float dt) {

	if (state==0) {
		m_Alpha += (1.0f - m_Alpha) * dt;
		if (m_Alpha > 0.98)
		{
			state = 1;
			time = clock() + 2000;
		}
	}
	else if (state == 1) {

		if (clock() > time) {
			state = 3;
		}

	}
	else if (state == 3) {

		m_Alpha += (0.0f - m_Alpha) * dt;
		if (m_Alpha < 0.03)
		{
			state = 0;
			m_CurrentImg++;
			if (m_CurrentImg >= m_Images.size())
			{
				m_CurrentImg = 0;
			}
		}

	}

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
	UIHelp::DrawImage(glm::vec2(mx, my), glm::vec2(mx * 2, my * 2), m_Images[m_CurrentImg], glm::vec4(1, 1, 1, m_Alpha), 0, 1.0f);
	UIHelp::End();


}

void IntroState::EndState() {

}