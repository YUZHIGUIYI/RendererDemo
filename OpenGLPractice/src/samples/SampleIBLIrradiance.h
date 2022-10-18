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
	class SampleIBLIrradiance : public Sample
	{
	public:
		SampleIBLIrradiance();
		~SampleIBLIrradiance();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

		void Init(RenderScene* scenebuffer);

	private:
		void renderSphere();
		void renderCube();

	private:
		bool Done;

		unsigned int cubeVAO, cubeVBO;
		unsigned int sphereVAO, indexCount;

		unsigned int captureFBO, captureRBO;

		unsigned int hdrTexture;

		unsigned int envCubemap;

		unsigned int irradianceMap;

		int scrWidth, scrHeight;
		int nrRows, nrColumns;
		float spacing;
		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightColors;
		glm::mat4 projection;
		glm::mat4 captureProjection;
		std::vector<glm::mat4> captureViews;

		std::unique_ptr<Renderer::Shader> pbrShader;
		std::unique_ptr<Renderer::Shader> equirectangularToCubemapShader;
		std::unique_ptr<Renderer::Shader> irradianceShader;
		std::unique_ptr<Renderer::Shader> backgroundShader;

		static unsigned int m_Width;
		static unsigned int m_Height;
	};
}