#pragma once

#include "Sample.h"
#include "Shader.h"
#include <memory>

namespace sample
{
	class SampleMSAA : public Sample
	{
	public:
		SampleMSAA();
		~SampleMSAA();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

		void Init(RenderScene* scenebuffer);

	private:
		bool Done;

		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Renderer::Shader> m_ShaderScreen;

		unsigned int cubeVAO, cubeVBO;
		unsigned int quadVAO, quadVBO;

		unsigned int framebuffer;
		unsigned int textureColorBufferMultiSampled;
		unsigned int rbo;

		unsigned int intermediateFBO;
		unsigned int screenTexture;

		static int m_Width;
		static int m_Height;
	};
}