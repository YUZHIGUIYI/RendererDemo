#pragma once

#include "Sample.h"
#include "Shader.h"
#include "Model.h"
#include <memory>

namespace sample
{
	class SampleRenderModel : public Sample
	{
	public:
		SampleRenderModel();
		~SampleRenderModel();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

	private:
		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Model> m_Model;

		float m_ClearColor[4];

		static int m_Width;
		static int m_Height;
	};
}