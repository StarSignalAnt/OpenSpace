#include "IPropertyEditor.h"

void IPropertyEditor::Update(float delta) {
	UpdateChildren(delta);
}

void IPropertyEditor::Render() {


	int cy = 0;
	int i = 0;
	for (auto g : m_Groups) {

		auto content_Height = g->GetMaxHeight();
	
		g->Set(glm::vec2(0,cy), g->GetSize());
		cy = cy + content_Height + 6;
	}
	RenderChildren();
}

void IPropertyEditor::AddGroup(PropertyGroup* group) {

	int sy = m_Groups.size() * 35;
	m_Groups.push_back(group);
	AddChild(group);
	group->Set(glm::vec2(0, sy),glm::vec2(m_Size.x,30));


}