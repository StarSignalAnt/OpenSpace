#include "GameFont.h"
#include <iostream>
#include "Texture2D.h"
#include <glm/gtc/matrix_transform.hpp>
#include "UIHelp.h"
#include <algorithm>
#include <glad/glad.h>

GameFont::GameFont(std::string path,int size)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "Could not init FreeType library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, path.c_str(), 0, &face)) {
        std::cerr << "Failed to load font: " << path << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, size);

 //   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) { // Load first 128 ASCII characters
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "Failed to load glyph: " << c << std::endl;
            continue;
        }

        unsigned char* nBuf = (unsigned char *)malloc(face->glyph->bitmap.width * face->glyph->bitmap.rows * 4);

        for (int y = 0; y < face->glyph->bitmap.rows; y++) {
            for (int x = 0; x < face->glyph->bitmap.width; x++) {

                int loc1 = (y * face->glyph->bitmap.width) + x;
                int loc2 = (y * face->glyph->bitmap.width * 4) + (x * 4);
                char r = face->glyph->bitmap.buffer[loc1];
                nBuf[loc2] = r;
                nBuf[loc2 + 1] = r;
                nBuf[loc2 + 2] = r;
                if (r <-120) {
                    nBuf[loc2 + 3] = -124;
                    nBuf[loc2] = -124;
                    nBuf[loc2 + 1] = -124;
                    nBuf[loc2 + 2] = -124;
                    //nBuf[loc2] = 

                }
                else {
                    
                    nBuf[loc2] = r;
                    nBuf[loc2 + 1] = r;
                    nBuf[loc2 + 2] = r;
                    nBuf[loc2 + 3] = r;

                }


            }
        }

		Texture2D* charTex = new Texture2D(face->glyph->bitmap.width, face->glyph->bitmap.rows,nBuf,4);

        FontChar fc = {

            charTex,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<int>(face->glyph->advance.x)


        };

        

        // Store character info
       
        m_Chars.insert(std::pair<char,FontChar>(c, fc));
    }


   // glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

}
void GameFont::Render(glm::vec2 position, std::string text, glm::vec4 color,float scale) {
    // Activate corresponding render state
    // Assuming you have a shader program for text rendering
    // shader.use();
    // glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    // glActiveTexture(GL_TEXTURE0);
    // glBindVertexArray(VAO);
    
    float x = position.x;
    float y = position.y;

    // Find the tallest character in the string to establish a consistent top line
    float maxHeight = 0.0f;
    for (auto c = text.begin(); c != text.end(); c++) {
        FontChar ch = m_Chars[*c];
        // Manual max function
        if (ch.Size.y > maxHeight) {
            maxHeight = ch.Size.y;
        }
    }

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        FontChar ch = m_Chars[*c];
        float xpos = x + ch.Bearing.x * scale;

        // Calculate ypos so characters are aligned by their top edge at position.y
        float ypos = y + (maxHeight - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // Update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        if (ch.m_Image != nullptr) {

            if (w == 0 || h == 0) {

            }
            else {

                //std::cout << "C:" << c << std::endl;
                UIHelp::DrawCharacter(glm::vec2(xpos, ypos), glm::vec2(w, h), ch.m_Image, color);
                // Now advance cursors for next glyph (advance is number of 1/64 pixels)
            }
            }
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }

}

float GameFont::StrWidth(const std::string& text, float scale) {
    if (text.empty()) {
        return 0.0f;
    }

    float totalWidth = 0.0f;
    std::string::const_iterator c;

    for (c = text.begin(); c != text.end(); c++) {
        FontChar ch = m_Chars[*c];
        totalWidth += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }

    return totalWidth;
}

// Calculate the pixel height of a string
float GameFont::StrHeight(const std::string& text, float scale) {
    if (text.empty()) {
        return 0.0f;
    }

    float maxHeight = 0.0f;
    std::string::const_iterator c;

    for (c = text.begin(); c != text.end(); c++) {
        FontChar ch = m_Chars[*c];
        // Find the tallest character in the string
        if (ch.Size.y > maxHeight) {
            maxHeight = ch.Size.y;
        }
    }

    return maxHeight * scale;
}