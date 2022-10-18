#pragma once

#include "Sample.h"
#include "Shader.h"
#include "Model.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace sample
{
	class SampleIBLSpecular : public Sample
	{
	public:
		SampleIBLSpecular();
		~SampleIBLSpecular();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

		void Init(RenderScene* scenebuffer);

	private:
		void renderSphere();
		void renderCube();
		void renderQuad();

	private:
		bool Done;

		unsigned int sphereVAO, indexCount;
		unsigned int cubeVAO, cubeVBO;
		unsigned int quadVAO, quadVBO;

		unsigned int captureFBO, captureRBO;
		unsigned int envCubemap;
		unsigned int irradianceMap;
		unsigned int prefilterMap;
		unsigned int maxMipLevels;

		unsigned int brdfLUTTexture;
		unsigned int hdrTexture;

		int nrRows, nrColumns;
		float spacing;

		glm::mat4 captureProjection;
		glm::mat4 projection;

		std::vector<glm::mat4> captureViews;
		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightColors;

		std::unique_ptr<Renderer::Shader> pbrShader;
		std::unique_ptr<Renderer::Shader> equirectangularToCubemapShader;
		std::unique_ptr<Renderer::Shader> irradianceShader;
		std::unique_ptr<Renderer::Shader> prefilterShader;
		std::unique_ptr<Renderer::Shader> brdfShader;
		std::unique_ptr<Renderer::Shader> backgroundShader;

		static unsigned int m_Width;
		static unsigned int m_Height;
	};
}