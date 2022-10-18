#pragma once

#include "Sample.h"
#include "Shader.h"
#include <memory>
#include <vector>

namespace sample
{
	class SampleGeometryHouse : public Sample
	{
	public:
		SampleGeometryHouse();
		~SampleGeometryHouse();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

	private:
		std::unique_ptr<Renderer::Shader> m_Shader;

		unsigned int VBO, VAO;
	};
}