#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include "Model.h"
#include <memory>
#include <vector>

namespace sample
{
	class SampleAdvancedGLSL : public Sample
	{
	public:
		SampleAdvancedGLSL();
		~SampleAdvancedGLSL();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera) override;
		void OnImGuiRenderer() override;

	private:
		unsigned int cubeVAO, cubeVBO;
		unsigned int uboMatrices;
		unsigned int uniformBlockIndex[4];

		std::unique_ptr<Renderer::Shader> m_Shader[4];

		float m_ClearColor[4];

		static int m_Width;
		static int m_Height;
	};
}