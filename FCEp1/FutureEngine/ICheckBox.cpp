#include "ICheckBox.h"
#include "UIHelp.h"
#include "UITheme.h"
#include "GameUI.h"

void ICheckBox::Update(float delta) {

}

void ICheckBox::Render() {

	auto theme = GameUI::GetTheme();
	m_Size = glm::vec2(20,20);

	UIHelp::DrawRect(GetRenderPosition(), glm::vec2(20, 20), theme->GetWindowLight());
	UIHelp::DrawRect(GetRenderPosition() + glm::vec2(1, 1), glm::vec2(18, 18), theme->GetWindowDark());
	if (m_Checked) {
		UIHelp::DrawRect(GetRenderPosition() + glm::vec2(3, 3), glm::vec2(14, 14), theme->GetWindowLight()*2.5f);
	}
	UIHelp::DrawText(GetRenderPosition() + glm::vec2(38, 4), m_Text, glm::vec4(1, 1, 1, 1));




}

void ICheckBox::OnMouseDown(int button) {

	m_Checked = m_Checked ? false : true;
	Checked();

}