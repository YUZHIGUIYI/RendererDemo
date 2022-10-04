#include "TextRenderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ResourceManager.h"
#include "Log.h"

namespace breakout
{

	TextRenderer::TextRenderer(unsigned int width, unsigned int height)
	{
		// load and configure shader
		this->TextShader = ResourceManager::LoadShader("res/shaders/breakout/Text2D.vert", 
			"res/shaders/breakout/Text2D.frag", "", "text");
		this->TextShader.Bind();
		this->TextShader.SetUniformMat4f("projection", 1, 
			glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f));
		this->TextShader.SetUniform1i("text", 0);

		// configure VAO/VBO for texture quads
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	TextRenderer::~TextRenderer()
	{
		glDeleteVertexArrays(1, &this->VAO);
		glDeleteBuffers(1, &this->VBO);
	}

	void TextRenderer::Load(const std::string& font, unsigned int fontSize)
	{
		// first clear the previously loaded Characters
		this->Characters.clear();
		// then initialize and load the FreeType library
		FT_Library ft;
		if (FT_Init_FreeType(&ft)) // all functions return a value different than 0 whenever an error occurred
			RD_ERROR("FreeType Error: Could not init FreeType Library");
		// load font as face
		FT_Face face;
		if (FT_New_Face(ft, font.c_str(), 0, &face))
			RD_ERROR("FreeType Error: Failed to load font");
		// set size to load glyphs as
		FT_Set_Pixel_Sizes(face, 0, fontSize);
		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		// then for the first 128 ASCII characters, pre-load/compile their characters and store them
		for (char c = 0; c < 128; ++c)
		{
			// load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				RD_ERROR("FreeType Error: Failed to load Glyph");
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};

			Characters.insert({ c, character });
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		// destroy FreeType once we're finished
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}

	void TextRenderer::RenderText(const std::string& text, float x, float y, float scale, 
		glm::vec3 color /*= glm::vec3(1.0f)*/)
	{
		// activate corresponding render state
		this->TextShader.Bind();
		this->TextShader.SetUniformVec3f("textColor", color);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(this->VAO);

		// iterate through all characters
		for (const auto& c : text)
		{
			Character& ch = Characters[c];

			float xpos = x + ch.Bearing.x * scale;
			float ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;  // ???
			//float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;
			// update VBO for each character
			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 0.0f },

				{ xpos,     ypos + h,   0.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 0.0f }
			};
			// render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// now advance cursors for next glyph
			x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}