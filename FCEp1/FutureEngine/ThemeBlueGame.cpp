#include "ThemeBlueGame.h"
#include "Texture2D.h"

void ThemeBlueGame::SetupTheme() {

	m_Background = glm::vec4(0, 0.455, 0.455, 1.0);
	m_Foreground = glm::vec4(0, 0.55, 0.55, 1.0);
	m_Button = new Texture2D("engine/themes/bluegame/rect.png");
	m_ScrollButton = new Texture2D("engine/ui/softrect.png");
	m_WindowTitleBar = glm::vec4(0, 0.202, 0.202, 1.0);
	m_WindowBlur = true;
	m_WindowLight = glm::vec4(0, 0.1455, 0.1455, 1.0);
	m_WindowDark = glm::vec4(0, 0.102, 0.102, 1.0);
	m_WindowAlpha = 0.65;
	m_GameTheme = true;

}