#include "ThemeDark.h"
#include "Texture2D.h"

void ThemeDark::SetupTheme() {

	m_Background = glm::vec4(0.502, 0.455, 0.455,1.0);
	m_Foreground = glm::vec4(0.5, 0.5, 0.5, 1.0);
	m_Button = new Texture2D("engine/ui/buttonframe.png");
	m_ScrollButton = new Texture2D("engine/ui/softrect.png");
	m_WindowTitleBar = glm::vec4(0.102, 0.102, 0.102, 1.0);
	m_WindowBlur = true;
	m_WindowLight = glm::vec4(0.145, 0.1455, 0.1455, 1.0);
	m_WindowDark = glm::vec4(0.102, 0.102, 0.102, 1.0);
	m_WindowAlpha = 0.65;

}