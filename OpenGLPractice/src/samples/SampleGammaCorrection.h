#pragma once

#include "Sample.h"
#include "Shader.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace sample
{
	class SampleGammaCorrection : public Sample
	{
	public:
		SampleGammaCorrection();
		~SampleGammaCorrection();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

	private:
		unsigned int planeVAO, planeVBO;

		std::unique_ptr<Renderer::Shader> m_Shader;
		unsigned int m_TextureFloorID;
		unsigned int m_TextureFloorGammaID;

		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightColors;

		bool gammaEnabled;

		static int m_Width;
		static int m_Height;

	private:
		unsigned int loadTexture(const char* path, bool gammaCorrection);
	};
}