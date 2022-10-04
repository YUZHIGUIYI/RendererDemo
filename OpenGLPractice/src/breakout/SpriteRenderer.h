#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "Texture2D.h"
#include "Shader.h"

namespace breakout
{
	class SpriteRenderer
	{
	public:
		SpriteRenderer(Renderer::Shader& shader);
		~SpriteRenderer();

		// Renders a defined quad textured with given sprite
		void DrawSprite(Renderer::Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), 
			float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

	private:
		// Render state
		Renderer::Shader shader;
		unsigned int quadVAO;
		// Initializes and configures the quad's buffer and vertex attributes
		void InitRenderData();
	};
}