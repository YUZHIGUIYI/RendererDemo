#pragma once

#include "Sample.h"
#include "Shader.h"
#include "Model.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace sample
{
	class SampleIBLSpecularTexture : public Sample
	{
	public:
		SampleIBLSpecularTexture();
		~SampleIBLSpecularTexture();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera) override;
		void OnImGuiRenderer() override;

	private:
		void renderSphere();
		void renderCube();
		void renderQuad();
		unsigned int loadTexture(const char* path);

	private:
		unsigned int sphereVAO;
		unsigned int indexCount;
		unsigned int cubeVAO, cubeVBO;
		unsigned int quadVAO, quadVBO;

		unsigned int ironAlbedoMap, ironNormalMap, ironMetallicMap, ironRoughnessMap, ironAOMap;
		unsigned int goldAlbedoMap, goldNormalMap, goldMetallicMap, goldRoughnessMap, goldAOMap;
		unsigned int grassAlbedoMap, grassNormalMap, grassMetallicMap, grassRoughnessMap, grassAOMap;
		unsigned int plasticAlbedoMap, plasticNormalMap, plasticMetallicMap, plasticRoughnessMap, plasticAOMap;
		unsigned int wallAlbedoMap, wallNormalMap, wallMetallicMap, wallRoughnessMap, wallAOMap;

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