#include "MenuState.h"
#include "Starfield.h"
#include "Texture2D.h"
#include "FutureApp.h"
#include "GameFont.h"
#include "GameUI.h"
#include "IWindow.h"
#include "IButton.h"
#include "FutureApp.h"
#include "SoundLib.h"


void MenuState::InitState() {

	m_Stars = new Starfield(512);
	m_Title = new Texture2D("content/images/labels/openspace.png");
	m_Font = new GameFont("engine/ui/5_uifont.ttf",64);
	m_UI = new GameUI;

	int sw = FutureApp::m_Inst->GetWidth();
	int sh = FutureApp::m_Inst->GetHeight();

	auto win = new IWindow("Destiny Saga 1 : Flowerfall", glm::vec2(sw / 2 - 200, sh / 2 - 125), glm::vec2(400, 240), false, false);

	//m_UI->AddWindow(win);


	auto new_g = new IButton("New Adventure", glm::vec2(60, 500+30), glm::vec2(365, 30));
	auto load_g = new IButton("Load Adventure", glm::vec2(60, 500+70), glm::vec2(365, 30));
	auto settings = new IButton("Settings", glm::vec2(60, 500+110), glm::vec2(365, 30));
	auto quit = new IButton("Exit Game", glm::vec2(60, 500+150), glm::vec2(365, 30));

	m_UI->GetRoot()->AddChild(new_g);
	m_UI->GetRoot()->AddChild(load_g);
	m_UI->GetRoot()->AddChild(settings);
	m_UI->GetRoot()->AddChild(quit);

	quit->SetOnClick([](void* data) {

		exit(1);

		});




	auto boot = FutureApp::m_Inst->SLib->loadSound("content/music/scene/menu/menusong.wav");
	m_Music = FutureApp::m_Inst->SLib->playSound(boot);
	m_Title = new Texture2D("content/images/labels/ds1.png");
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
//	m_Font->Render(glm::vec2(20, 20), "OpenSpace", glm::vec4(0.15f, 1, 1, 1), 1.0f);
	int tx = FutureApp::m_Inst->GetWidth() / 2;
	int ty = 50;

	UIHelp::DrawImage(glm::vec2(tx-192, ty), glm::vec2(192*2,192*2), m_Title, glm::vec4(1, 1, 1, 1));
	UIHelp::End();
	FutureApp::m_Inst->ClearZ();
	m_UI->RenderUI();



}

void MenuState::EndState() {

}