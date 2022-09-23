#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>

namespace sample
{
	class SampleFrameBuffers : public Sample
	{
	public:
		SampleFrameBuffers();
		~SampleFrameBuffers();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera) override;
		void OnImGuiRenderer() override;

	private:
		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Renderer::Shader> m_ShaderScreen;
		std::unique_ptr<Renderer::Texture> m_TextureCube;
		std::unique_ptr<Renderer::Texture> m_TextureFloor;

		unsigned int cubeVAO, cubeVBO;
		unsigned int planeVAO, planeVBO;
		unsigned int quadVAO, quadVBO;

		unsigned int FrameBuffer;
		unsigned int TexColorBuffer;
		unsigned int RBO;

		float m_ClearColor[4];

		static int m_Width;
		static int m_Height;
	};
}