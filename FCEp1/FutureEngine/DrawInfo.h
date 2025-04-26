#pragma once
#include "glm/common.hpp"
#include <glm/glm.hpp>  // Basic types (vec3, mat3, etc.)

class Texture2D;

class DrawInfo
{
public:

	DrawInfo() {
		m_X = new float[4];
		m_Y = new float[4];
		m_TexX = new float[4];
		m_TexY = new float[4];
		m_RealX = new float[4];
		m_RealY = new float[4];
		m_Extra = new float[4];
		m_View = new float[4];
	}
	void SetCoord(int coord, float x, float y);
	void SetRealCoord(int coord, float x, float y)
	{
		m_RealX[coord] = x;
		m_RealY[coord] = y;
	}
	void SetExtra(glm::vec4 extra)
	{
		m_Extra[0] = extra.x ;
		m_Extra[1] = extra.y;
		m_Extra[2] = extra.z;
		m_Extra[3] = extra.w;
	}
	void SetTexCoord(int coord, float x, float y);
	void SetColor(float r, float g, float b, float a);
	void SetColor(glm::vec4 color);
	void SetTexture(Texture2D* texture);
	void SetNormalTexture(Texture2D* texture);
	void SetZ(float z);
	glm::vec2 GetCoord(int coord) {
		//x = *m_X;
		//y = *m_Y;
		return glm::vec2(m_X[coord], m_Y[coord]);
	}
	glm::vec3 GetRealCoord(int coord) {

		return glm::vec3(m_RealX[coord], m_RealY[coord],0);

	}
	glm::vec4 GetExtra() {
		return glm::vec4(m_Extra[0], m_Extra[1], m_Extra[2], m_Extra[3]);

	}
	glm::vec4 GetView() {
		return glm::vec4(m_View[0], m_View[1], m_View[2], m_View[3]);
	}
	void SetView(glm::vec4 view) {

		m_View[0] = view.x;
		m_View[1] = view.y;
		m_View[2] = view.z;
		m_View[3] = view.w;

	}


	glm::vec2 GetTexCoord(int coord) {
		//x = *m_TexX;
		//y = *m_TexY;
		return glm::vec2(m_TexX[coord], m_TexY[coord]);
	}
	glm::vec4 GetColor() {
		return m_Color;
	}
	Texture2D* GetTexture() {
		return m_Texture;
	}
	
	Texture2D* GetNormalTexture() {
		return m_NormalTexture;
	}

	float GetZ() {
		return m_Z;
	}
	~DrawInfo() {
	
	}
	void Free();
	void SetAux(Texture2D* tex) {
		m_AuxTexture = tex;
	}
	Texture2D* GetAux() {
		return m_AuxTexture;
	}

private:

	float* m_X;
	float* m_Y;
	float* m_RealX;
	float* m_RealY;
	float* m_Extra;
	float* m_View;
	float* m_TexX;
	float* m_TexY;
	float m_Z = 0.0f;
	glm::vec4 m_Color;
	Texture2D* m_Texture;
	Texture2D* m_NormalTexture;
	Texture2D* m_AuxTexture;



};

