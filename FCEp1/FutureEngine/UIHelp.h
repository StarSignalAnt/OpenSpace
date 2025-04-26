#pragma once
#include <glm/glm.hpp>
#include <string>

class Texture2D;

class UIHelp
{
public:
	static void setScissor(int x, int y, int width, int height, int windowHeight);
	static void RemoveScissor();
	static void InitHelp();
	static void DrawImageBlur(glm::vec2 pos, glm::vec2 size, Texture2D* image, glm::vec4 color, float blur = 0.1);
	static void DrawImage(glm::vec2 pos, glm::vec2 size, Texture2D* image,glm::vec4 color);
	static void DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
	static void DrawOutlineRect(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
	static void DrawCharacter(glm::vec2 pos, glm::vec2 size, Texture2D* image, glm::vec4 color);
	static void DrawText(glm::vec2 position, std::string text,glm::vec4 color,float scale=1.0);
	static float StrWidth(std::string text,float scale=1.0);
	static float StrHeight(std::string text,float scale=1.0);
	static void DrawImageWithBG(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
	static void ClearZ();
	static void Begin();
	static void End();
	static glm::vec4 m_Scissor;

};

