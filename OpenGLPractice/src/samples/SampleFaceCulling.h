#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>

namespace sample
{
	class SampleFaceCulling : public Sample
	{
	public:
		SampleFaceCulling();
		~SampleFaceCulling();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

	private:
		unsigned int cubeVAO, cubeVBO;

		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Renderer::Texture> m_Texture;

		float m_ClearColor[4];
		bool FaceCullingMode;

		static int m_Width;
		static int m_Height;
	};
}