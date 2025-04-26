#include "DrawInfo.h"

void DrawInfo::SetCoord(int coord, float x, float y)
{
	if (coord == 0)
	{
		*m_X = x;
		*m_Y = y;
	}
	else {
		m_X[coord] = x;
		m_Y[coord] = y;
	}
}

void DrawInfo::SetTexCoord(int coord, float x, float y)
{
	if (coord == 0)
	{
		*m_TexX = x;
		*m_TexY = y;
	}
	else {
		m_TexX[coord] = x;
		m_TexY[coord] = y;
	}
}


void DrawInfo::SetColor(float r, float g, float b, float a)
{
	m_Color = glm::vec4(r, g, b, a);
}

void DrawInfo::SetColor(glm::vec4 color)
{
	m_Color = color;
}

void DrawInfo::SetTexture(Texture2D* texture)
{
	m_Texture = texture;
}

void DrawInfo::SetNormalTexture(Texture2D* texture)
{
	m_NormalTexture = texture;
}

void DrawInfo::SetZ(float z)
{
	m_Z = z;
}

void DrawInfo::Free()
{
	delete m_X;
	delete m_Y;
	delete m_TexX;
	delete m_TexY;
	delete m_Extra;
	delete m_View;
	delete m_RealX;
	delete m_RealY;
}

