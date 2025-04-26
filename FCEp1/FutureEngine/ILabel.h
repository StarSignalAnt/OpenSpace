#pragma once
#include "IControl.h"
class ILabel : public IControl
{
public:

	ILabel(std::string text, glm::vec2 position) {

		m_Position = position;
		m_Text = text;
		SetIgnoreScissor(true);

	}

	void Update(float delta) override;
	void Render() override;

};

