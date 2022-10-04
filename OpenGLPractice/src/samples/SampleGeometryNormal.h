#pragma once

#include "Sample.h"
#include "Shader.h"
#include "Model.h"
#include <memory>
#include <vector>

namespace sample
{
	class SampleGeometryNormal : public Sample
	{
	public:
		SampleGeometryNormal();
		~SampleGeometryNormal();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera) override;
		void OnImGuiRenderer() override;

	private:
		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Renderer::Shader> m_ShaderNormal;
		std::unique_ptr<Model> m_Model;

		float m_ClearColor[4];
		bool m_ShowNormal;

		static int m_Width;
		static int m_Height;
	};
}
