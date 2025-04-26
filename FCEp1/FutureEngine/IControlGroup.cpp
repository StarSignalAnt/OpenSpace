#include "IControlGroup.h"

IControlGroup::IControlGroup() {

}

IControlGroup::IControlGroup(glm::vec2 position, glm::vec2 size) {

	m_Position = position;
	m_Size = size;

}

void IControlGroup::Update(float delta)
{

	UpdateChildren(delta);

	for (auto control : m_Children)
	{
	//	control->Update(delta);
	}

}

void IControlGroup::Render()
{

	RenderChildren();
	//for (auto control : m_Children)
	//{
//		control->Render();
//	}
}
