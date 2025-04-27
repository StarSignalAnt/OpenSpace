#include "MenuState.h"
#include "Starfield.h"
#include "Texture2D.h"
#include "FutureApp.h"
#include "GameFont.h"
#include "GameUI.h"
#include "IWindow.h"
#include "IButton.h"


void MenuState::InitState() {

	m_Stars = new Starfield(512);
	m_Title = new Texture2D("content/images/labels/openspace.png");
	m_Font = new GameFont("engine/ui/5_uifont.ttf",64);
	m_UI = new GameUI;

	int sw = FutureApp::m_Inst->GetWidth();
	int sh = FutureApp::m_Inst->GetHeight();

	auto win = new IWindow("OpenSpace", glm::vec2(sw / 2 - 200, sh / 2 - 175), glm::vec2(400, 240), false, false);

	m_UI->AddWindow(win);

	auto new_g = new IButton("New Adventure", glm::vec2(10, 30), glm::vec2(375, 30));
	auto load_g = new IButton("Load Adventure", glm::vec2(10, 70), glm::vec2(375, 30));
	auto settings = new IButton("Settings", glm::vec2(10, 110), glm::vec2(375, 30));
	auto quit = new IButton("Exit Game", glm::vec2(10, 150), glm::vec2(375, 30));

	win->AddClientControl(new_g);
	win->AddClientControl(load_g);
	win->AddClientControl(settings);
	win->AddClientControl(quit);

}

void MenuState::UpdateState(float dt) {

	m_Stars->Update(dt);
	m_UI->UpdateUI(dt);
}

void MenuState::RenderState() {

	m_Stars->Render();

	FutureApp::m_Inst->ClearZ();
	UIHelp::Begin();
	//UIHelp::DrawText(glm::vec2(20, 20), "OpenSpace", glm::vec4(0, 1, 1, 1), 3.0f);
	m_Font->Render(glm::vec2(20, 20), "OpenSpace", glm::vec4(0.15f, 1, 1, 1), 1.0f);
	UIHelp::End();
	FutureApp::m_Inst->ClearZ();
	m_UI->RenderUI();

}

void MenuState::EndState() {

}