#include "IImage.h"
#include "UIHelp.h"

void IImage::Update(float delta)
{

	UpdateChildren(delta);
}

void IImage::Render()
{

	auto renderPos = GetRenderPosition();

	UIHelp::DrawImage(renderPos, m_Size, m_Image, m_Color);
	RenderChildren();
}