#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include "Model.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace sample
{
	class SampleLightingTextured : public Sample
	{
	public:
		SampleLightingTextured();
		~SampleLightingTextured();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera) override;
		void OnImGuiRenderer() override;

	private:
		void renderSphere();

	private:
		unsigned int sphereVAO, indexCount;

		int nrRows, nrColumns;
		float spacing;

		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightColors;

		glm::mat4 projection;

		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Renderer::Texture> m_Albedo;
		std::unique_ptr<Renderer::Texture> m_Normal;
		std::unique_ptr<Renderer::Texture> m_Metallic;
		std::unique_ptr<Renderer::Texture> m_Roughness;
		std::unique_ptr<Renderer::Texture> m_Ao;

		static unsigned int m_Width;
		static unsigned int m_Height;
	};
}