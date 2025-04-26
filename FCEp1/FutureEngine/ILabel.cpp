#include "ILabel.h"
#include "UIHelp.h"

void ILabel::Update(float delta) {

}

void ILabel::Render() {


	m_Size.x = UIHelp::StrWidth(m_Text) + 40;
	m_Size.y = UIHelp::StrHeight(m_Text) + 40;

	UIHelp::DrawText(GetRenderPosition(), m_Text, glm::vec4(1, 1, 1, 1), 1.2f);

}