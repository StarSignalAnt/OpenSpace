#pragma once
#include <glm/glm.hpp>

enum MouseButton {
	MOUSE_BUTTON_LEFT = 0,
	MOUSE_BUTTON_RIGHT = 1,
	MOUSE_BUTTON_MIDDLE = 2
};

class IWindow;

class GameInput
{
public:

	static glm::vec2 MousePosition;
	static glm::vec3 MouseDelta;
	static bool Buttons[16];
	static bool Keys[512];
	static IWindow* m_Dragging;;


private:

};

