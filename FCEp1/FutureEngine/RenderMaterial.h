#pragma once

class Texture2D;

class RenderMaterial
{
public:

	void SetColor(Texture2D* texture)
	{
		m_Color = texture;
	}
	void SetNormal(Texture2D* normal)
	{
		m_Normal = normal;
	}
	Texture2D* GetColor() {
		return m_Color;
	}
	Texture2D* GetNormal() {
		return m_Normal;
	}

private:

	Texture2D* m_Color = nullptr;
	Texture2D* m_Normal = nullptr;
	Texture2D* m_Rough = nullptr;
	Texture2D* m_Metal = nullptr;

};

