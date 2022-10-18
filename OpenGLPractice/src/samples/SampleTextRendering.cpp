#include "SampleTextRendering.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>
#include "Log.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace sample
{

	SampleTextRendering::SampleTextRendering()
		: m_ColorTop(glm::vec3(0.3f, 0.7f, 0.9f)), m_ColorBot(glm::vec3(0.5f, 0.8f, 0.2f))
	{
		// OpenGL state
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// compile and setup the shader
		m_Shader = std::make_unique<Renderer::Shader>("res/shaders/freetype/Text.vert",
			"res/shaders/freetype/Text.frag");
		glm::mat4 projection = glm::ortho(0.0f, (float)m_Width, 0.0f, (float)m_Height);
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("projection", 1, projection);

		// FreeType
		FT_Library ft;
		// All functions return a value different than 0 whenever an error occurred
		if (FT_Init_FreeType(&ft))
		{
			RD_ERROR("FreeType Error: Could not init FreeType Library");
		}

		// find path to font
		std::string font_name = "res/fonts/Antonio-Bold.ttf";
		if (font_name.empty())
		{
			RD_ERROR("FreeType Error: Failed to load font_name");
		}

		// load font as face
		FT_Face face;
		if (FT_New_Face(ft, font_name.c_str(), 0, &face))
		{
			RD_ERROR("FreeType Error: Failed to load font");
		}
		else
		{
			// set size to load glyphs as
			FT_Set_Pixel_Sizes(face, 0, 48);

			// disable byte-alignment restriction
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			// load first 128 characters of ASCII set
			for (unsigned char c = 0; c < 128; ++c)
			{
				// Load character glyph 
				if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				{
					RD_ERROR("FreeType Error: Failed to load Glyph");
					continue;
				}
				// generate texture
				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,
					0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
				// set texture options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// row store character for later use
				Character character = { texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x) };
				m_Characters.insert({ c, character });
			}
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		// destroy FreeType once we are finished
		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		// configure VAO/VBO for texture quads
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	SampleTextRendering::~SampleTextRendering()
	{
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	void SampleTextRendering::OnUpdate(float deltaTime)
	{

	}

	void SampleTextRendering::OnRender(const Camera& camera, RenderScene* scenebuffer)
	{
		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		RenderText("This is sample text render", 25.0f, 25.0f, 1.0f, m_ColorBot);
		RenderText("(C) Renderer Demo", 540.0f, 570.0f, 0.5f, m_ColorTop);
	}

	void SampleTextRendering::OnImGuiRenderer()
	{
		ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.7f, 1.0f), "Text Rendering");
		ImGui::ColorEdit3("Change Color Top", glm::value_ptr(m_ColorTop));
		ImGui::ColorEdit3("Change Color Bottom", glm::value_ptr(m_ColorBot));
	}

	void SampleTextRendering::RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color)
	{
		m_Shader->Bind();
		m_Shader->SetUniformVec3f("textColor", color);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

		// iterate through all characters
		for (const auto& c : text)
		{
			Character& ch = m_Characters[c];

			float xpos = x + ch.Bearing.x * scale;
			float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;
			// update VBO for each character
			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};
			// render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	unsigned int SampleTextRendering::m_Width = 800;

	unsigned int SampleTextRendering::m_Height = 600;

}