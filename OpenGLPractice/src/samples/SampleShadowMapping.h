#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <glm/glm.hpp>

namespace sample
{
	class SampleShadowMapping : public Sample
	{
	public:
		SampleShadowMapping();
		~SampleShadowMapping();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera) override;
		void OnImGuiRenderer() override;

	private:
		void renderScene(Renderer::Shader& shader);
		void renderCube();
		void renderQuad();

	private:
		unsigned int planeVAO, planeVBO;
		unsigned int cubeVAO, cubeVBO;
		unsigned int quadVAO, quadVBO;

		unsigned int depthMapFBO;
		unsigned int depthMap;

		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Renderer::Shader> m_ShaderSimpleDepth;
		std::unique_ptr<Renderer::Shader> m_ShaderDebugDepthQuad;

		std::unique_ptr<Renderer::Texture> m_TextureWood;

		glm::vec3 lightPos;

		static unsigned int m_Width;
		static unsigned int m_Height;
		static unsigned int m_ShadowWidth;
		static unsigned int m_ShadowHeight;
	};
}