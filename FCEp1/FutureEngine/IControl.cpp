#include "IControl.h"
#include <glad/glad.h>
#include "FutureApp.h"
#include "IWindow.h"
#include "GameUI.h"
#include "UIHelp.h"
#include "GameInput.h"

glm::vec2 IControl::GetRenderPosition() {

	glm::vec2 root = glm::vec2(0, 0);

	if (m_RootControl != nullptr) {
		root = m_RootControl->GetRenderPosition();
		

		root = root += m_RootControl->GetOffset();
	}

	return root + m_Position;

}

void IControl::UpdateChildren(float delta) {
	for (auto child : m_Children) {
		child->Update(delta);
	}
}

void IControl::PreRenderChildren() {

    for (auto child : m_Children) {

        child->PreRender();

    }
}

glm::vec4 IControl::GetScissor() {

	glm::vec4 root = glm::vec4(0, 0, FutureApp::m_Inst->GetWidth(), FutureApp::m_Inst->GetHeight());

	if (GetRoot() != nullptr) {

		root = GetRoot()->GetScissor();

	}

	auto rpos = GetRenderPosition();
	auto rsize = GetSize();
	float sx, sy,ex,ey;

	sx = rpos.x;
	sy = rpos.y;
	ex = sx + rsize.x;
	ey = sy + rsize.y;

	if (sx < root.x)
	{
		//int dif = root.x - sx;
		sx = root.x;
	}
	if (sy < root.y) {
		sy = root.y;
	}
	if (ex > (root.x + root.z))
	{
		ex = root.x + root.z;
	}
	if (ey > (root.y + root.w))
	{
		ey = root.y + root.w;
	}


	return glm::vec4(sx,sy,ex-sx,ey-sy);

}

void IControl::RenderChildren() {
    // Skip if no children
    if (m_Children.empty()) {
        return;
    }

    auto rpos = GetRenderPosition();
    auto size = GetSize();
    auto offset = GetOffset();

    // Store current scissor state
   

    // Render all children
    for (auto child : m_Children) {
		auto s = child->GetScissor();

		if (GetIgnoreScissor()) {
			UIHelp::RemoveScissor();
		}
		else {
			UIHelp::setScissor(s.x, s.y, s.z, s.w, FutureApp::m_Inst->GetHeight());
		}
        // Render the child
        child->Render();
    }

	UIHelp::RemoveScissor();

}
    

int IControl::GetMaxHeight() {

	int max_y = 0;

	for (auto control : m_Children) {

		auto controlY = control->GetPosition().y + control->GetSize().y;

		if (controlY > max_y) {
			max_y = controlY;
		}

	}

	return max_y;

}


int IControl::GetMaxWidth() {

	int max_x = 0;

	for (auto control : m_Children) {

		auto controlX = control->GetPosition().x + control->GetSize().x;

		if (controlX > max_x) {
			max_x = controlX;
		}

	}

	return max_x;

}

void IControl::setScissor(int x, int y, int width, int height, int windowHeight) {
	// Convert from left-top (0,0) to OpenGL's left-bottom (0,0)
	// by flipping the Y coordinate
	int openglY = windowHeight - (y + height);

	// Set the scissor test
	glEnable(GL_SCISSOR_TEST);
	glScissor(x, openglY, width, height);
}

bool IControl::IsWindow(bool title_only) {

	auto current = this;

	while (current != nullptr) {

		auto window = dynamic_cast<IWindow*>(current);
		if (window != nullptr)
		{

			if (title_only) {
				if (GameInput::MousePosition.y > window->GetRenderPosition().y && GameInput::MousePosition.y < window->GetRenderPosition().y + 20) {
					return true;
				}
				return false;
			}

			return true;
		}
		current = current->GetRoot();

	}

	return false;

}

IWindow* IControl::GetWindow() {

	auto current = this;

	while (current != nullptr) {

		auto window = dynamic_cast<IWindow*>(current);
		if (window != nullptr) return window;

		current = current->GetRoot();

	}

	return nullptr;

}