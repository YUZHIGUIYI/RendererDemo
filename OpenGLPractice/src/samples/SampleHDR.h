#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace sample
{
	class SampleHDR : public Sample
	{
	public:
		SampleHDR();
		~SampleHDR();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera) override;
		void OnImGuiRenderer() override;

	private:
		void renderCube();
		void renderQuad();

	private:
		unsigned int cubeVAO;
		unsigned int cubeVBO;
		unsigned int quadVAO;
		unsigned int quadVBO;

		unsigned int hdrFBO;
		unsigned int colorBuffer;
		unsigned int rboDepth;

		bool enableHDR;
		float exposure;

		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightColors;

		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Renderer::Shader> m_ShaderHDR;

		std::unique_ptr<Renderer::Texture> m_TextureWood;

		static unsigned int m_Width;
		static unsigned int m_Height;
	};
}