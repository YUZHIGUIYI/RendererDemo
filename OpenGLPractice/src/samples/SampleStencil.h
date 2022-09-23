#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>

namespace sample
{
	class SampleStencil : public Sample
	{
	public:
		SampleStencil();
		~SampleStencil();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera) override;
		void OnImGuiRenderer() override;

	private:
		std::unique_ptr<Renderer::Shader> m_ShaderOne;
		std::unique_ptr<Renderer::Shader> m_ShaderTwo;
		std::unique_ptr<Renderer::Texture> m_TextureOne;
		std::unique_ptr<Renderer::Texture> m_TextureTwo;

		float m_ClearColor[4];

		unsigned int cubeVAO, cubeVBO;
		unsigned int planeVAO, planeVBO;

		static int m_Width;
		static int m_Height;
	};
}
