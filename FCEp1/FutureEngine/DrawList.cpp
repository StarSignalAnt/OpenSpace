#include "DrawList.h"
#include "DrawInfo.h"
void DrawList::Add(DrawInfo* info)
{
	m_DrawList.push_back(info);
}

void DrawList::SetTexture(Texture2D* texture)
{
	m_Texture = texture;
}

Texture2D* DrawList::GetTexture()
{
	return m_Texture;
}

void DrawList::Free() {

	for (auto info : m_DrawList)
	{
		info->Free();
		delete info;
	}

}