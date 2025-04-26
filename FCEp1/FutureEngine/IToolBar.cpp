#include "IToolBar.h"
#include "UIHelp.h"
#include "IButton.h"

void IToolBar::Update(float delta) {


	UpdateChildren(delta);

}

void IToolBar::Render() {


	UIHelp::DrawRect(GetRenderPosition(), GetSize(), glm::vec4(0.145, 0.1455, 0.1455, 1.0));
	RenderChildren();

}

IButton* IToolBar::AddButton(std::string text) {


	auto button = new IButton(text, glm::vec2(m_NextX, 5), glm::vec2(UIHelp::StrWidth(text) + 10, 20));
	AddChild(button);
	m_NextX += UIHelp::StrWidth(text) + 20;
	return button;

}

IButton* IToolBar::AddButton(Texture2D* icon) {


	auto button = new IButton(icon, glm::vec2(m_NextX, 5),glm::vec2(30, 30));
	AddChild(button);
	m_NextX += 40;
	return button;

}

void IToolBar::Seperator(int width) {

	m_NextX += width;

}