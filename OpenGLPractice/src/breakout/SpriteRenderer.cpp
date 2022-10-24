#include "SpriteRenderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace breakout
{

	SpriteRenderer::SpriteRenderer(Renderer::Shader* shader)
	{
		this->shader = shader;
		InitRenderData();
	}

	SpriteRenderer::~SpriteRenderer()
	{
		glDeleteVertexArrays(1, &this->quadVAO);
		glDeleteBuffers(1, &this->quadVBO);
	}

	void SpriteRenderer::DrawSprite(Renderer::Texture2D& texture, glm::vec2 position, 
		glm::vec2 size /*= glm::vec2(10.0f, 10.0f)*/, float rotate /*= 0.0f*/, glm::vec3 color /*= glm::vec3(1.0f)*/)
	{
		// prepare transformations
		this->shader->Bind();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

		model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
		model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
		model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

		model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

		this->shader->SetUniformMat4f("model", 1, model);

		// render textured quad
		this->shader->SetUniformVec3f("spriteColor", color);

		glActiveTexture(GL_TEXTURE0);
		texture.Bind();

		glBindVertexArray(this->quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void SpriteRenderer::InitRenderData()
	{
		// configure VAO/VBO
		float vertices[] = {
			// pos      // tex
			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,

			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f
		};

		glGenVertexArrays(1, &this->quadVAO);
		glGenBuffers(1, &quadVBO);

		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(this->quadVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

}