#pragma once

#include "Sample.h"
#include "Shader.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

namespace sample
{
	struct Character
	{
		unsigned int TextureID;		// Id handle of the glyph texture
		glm::ivec2 Size;			// Size of glyph
		glm::ivec2 Bearing;			// Offset from baseline to left/top of glyph
		unsigned int Advance;		// Horizontal offset to advance to next glyph
	};

	class SampleTextRendering : public Sample
	{
	public:
		SampleTextRendering();
		~SampleTextRendering();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

	private:
		void RenderText(const std::string& text, float x, float y,
			float scale, const glm::vec3& color);

	private:
		bool Done = false;

		unsigned int VAO, VBO;

		glm::vec3 m_ColorTop;
		glm::vec3 m_ColorBot;

		std::unordered_map<char, Character> m_Characters;

		std::unique_ptr<Renderer::Shader> m_Shader;

		static unsigned int m_Width;
		static unsigned int m_Height;
	};
}