#include "GameInput.h"
#include "IWindow.h"

glm::vec2 GameInput::MousePosition = glm::vec2(0, 0);
glm::vec3 GameInput::MouseDelta = glm::vec3(0, 0, 0);
bool GameInput::Buttons[16] = { false };
bool GameInput::Keys[512] = { false };


IWindow* GameInput::m_Dragging = nullptr;