#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <glm/glm.hpp>

namespace sample
{
	class SamplePointShadows : public Sample
	{
	public:
		SamplePointShadows();
		~SamplePointShadows();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera) override;
		void OnImGuiRenderer() override;

	private:
		void renderScene(Renderer::Shader& shader);
		void renderCube();

	private:
		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Renderer::Shader> m_ShaderSimpleDepth;

		std::unique_ptr<Renderer::Texture> m_TextureWood;

		unsigned int cubeVAO, cubeVBO;
		unsigned int depthMapFBO;
		unsigned int depthCubemap;

		bool shadows;

		glm::vec3 lightPos;

		static unsigned int m_Width, m_Height;
		static unsigned int m_ShadowWidth, m_ShadowHeight;
	};
}