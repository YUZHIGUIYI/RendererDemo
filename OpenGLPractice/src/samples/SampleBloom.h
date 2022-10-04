#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace sample
{
	class SampleBloom : public Sample
	{
	public:
		SampleBloom();
		~SampleBloom();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera) override;
		void OnImGuiRenderer() override;

	private:
		void renderCube();
		void renderQuad();

	private:
		unsigned int cubeVAO, cubeVBO;
		unsigned int quadVAO, quadVBO;

		unsigned int hdrFBO;
		unsigned int colorBuffers[2];
		unsigned int rboDepth;
		unsigned int attachments[2];
		unsigned int pingpongFBO[2];
		unsigned int pingpongColorbuffers[2];

		bool bloom;
		float exposure;

		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightColors;

		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Renderer::Shader> m_ShaderLight;
		std::unique_ptr<Renderer::Shader> m_ShaderBlur;
		std::unique_ptr<Renderer::Shader> m_ShaderBloomFinal;

		std::unique_ptr<Renderer::Texture> m_TextureWood;
		std::unique_ptr<Renderer::Texture> m_TextureContainer;

		static unsigned int m_Width;
		static unsigned int m_Height;
	};
}