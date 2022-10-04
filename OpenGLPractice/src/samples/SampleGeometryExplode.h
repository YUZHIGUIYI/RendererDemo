#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include "Model.h"
#include <memory>
#include <vector>

namespace sample
{
	class SampleGeometryExplode : public Sample
	{
	public:
		SampleGeometryExplode();
		~SampleGeometryExplode();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera) override;
		void OnImGuiRenderer() override;

	private:
		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Model> m_Model;

		float m_ClearColor[4];

		static int m_Width;
		static int m_Height;
	};
}
