#pragma once

#include "Sample.h"
#include "Shader.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace sample
{
	class SampleInstancing : public Sample
	{
	public:
		SampleInstancing();
		~SampleInstancing();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera) override;
		void OnImGuiRenderer() override;

	private:
		unsigned int quadVAO, quadVBO;
		unsigned int instanceVBO;

		glm::vec2 translations[100];

		std::unique_ptr<Renderer::Shader> m_Shader;

		float m_ClearColor[4];

		static int m_Width;
		static int m_Height;
	};
}