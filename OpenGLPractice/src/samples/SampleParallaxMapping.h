#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <glm/glm.hpp>

namespace sample
{
	class SampleParallaxMapping : public Sample
	{
	public:
		SampleParallaxMapping();
		~SampleParallaxMapping();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

	private:
		void renderQuad();

	private:
		unsigned int quadVAO;
		unsigned int quadVBO;

		float heightScale = 0.1f;

		glm::vec3 lightPos;

		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Renderer::Texture> m_TextureDiffuseMap;
		std::unique_ptr<Renderer::Texture> m_TextureNormalMap;
		std::unique_ptr<Renderer::Texture> m_TextureHeightMap;

		static unsigned int m_Width;
		static unsigned int m_Height;
	};
}