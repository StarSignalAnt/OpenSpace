#pragma once
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H
#include <map>
#include <glm/glm.hpp>
#include <string>


class Texture2D;

struct FontChar {

	Texture2D* m_Image = nullptr;
	glm::ivec2 Size;   // Width and height of the glyph
	glm::ivec2 Bearing; // Offset from baseline
	int Advance;
};

class GameFont
{
public:

	GameFont(std::string path,int size);
	void Render(glm::vec2 position,std::string text, glm::vec4 color,float scale);

	float StrWidth(const std::string& text, float scale = 1.0f);
	float StrHeight(const std::string& text, float scale = 1.0f);


private:

	std::map<char, FontChar> m_Chars;


};

