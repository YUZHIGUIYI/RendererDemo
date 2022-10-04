#pragma once

#include "Sample.h"
#include "Shader.h"
#include "Texture.h"
#include <memory>
#include <glm/glm.hpp>

namespace sample
{
	class SampleBlinnPhong : public Sample
	{
	public:
		SampleBlinnPhong();
		~SampleBlinnPhong();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera) override;
		void OnImGuiRenderer() override;

	private:
		unsigned int planeVAO, planeVBO;
		bool selectBlinnPhong;

		glm::vec3 lightPos;

		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Renderer::Texture> m_Texture;

		static int m_Width;
		static int m_Height;
	};
}
