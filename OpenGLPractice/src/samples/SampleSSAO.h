#pragma once

#include "Sample.h"
#include "Shader.h"
#include "Model.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace sample
{
	class SampleSSAO : public Sample
	{
	public:
		SampleSSAO();
		~SampleSSAO();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

		void Init(RenderScene* scenebuffer);

	private:
		void renderQuad();
		void renderCube();
		float lerp(float a, float b, float f);

	private:
		bool Done;

		unsigned int cubeVAO, cubeVBO;
		unsigned int quadVAO, quadVBO;

		unsigned int gBuffer;
		unsigned int gPosition, gNormal, gAlbedo;
		unsigned int attachments[3];
		unsigned int rboDepth;

		unsigned int ssaoFBO, ssaoBlurFBO;
		unsigned int ssaoColorBuffer, ssaoColorBufferBlur;

		unsigned int noiseTexture;

		glm::vec3 lightPos;
		glm::vec3 lightColor;

		std::vector<glm::vec3> ssaoKernel;
		std::vector<glm::vec3> ssaoNoise;

		std::unique_ptr<Renderer::Shader> m_ShaderGeometryPass;
		std::unique_ptr<Renderer::Shader> m_ShaderLightingPass;
		std::unique_ptr<Renderer::Shader> m_ShaderSSAO;
		std::unique_ptr<Renderer::Shader> m_ShaderSSAOBlur;

		std::unique_ptr<Model> m_Model;

		int kernelSize;
		float kernelRadius;

		static unsigned int m_Width;
		static unsigned int m_Height;
	};
}