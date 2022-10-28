#pragma once

#include "Sample.h"
#include "Shader.h"
#include <memory>
#include <random>

namespace sample
{
	class SampleCSM : public Sample
	{
	public:
		SampleCSM();
		~SampleCSM();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

		void Init(RenderScene* scenebuffer);

	private:
		unsigned int LoadTexture(const char* path);
		void NRenderScene(Renderer::Shader& shader);
		void RenderCube();
		void RenderQuad();
		std::vector<glm::mat4> GetLightSpaceMatrices(const Camera& camera);
		glm::mat4 getLightSpaceMatrix(const Camera& camera, const float nearPlane, const float farPlane);
		std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& projview);
		std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
		void DrawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Renderer::Shader* shader);

	private:
		bool showQuad;
		bool Done;
		std::random_device device;
		std::mt19937 generator;
		std::vector<glm::mat4> lightMatricesCache;

		std::vector<float> shadowCascadeLevels;

		std::unique_ptr<Renderer::Shader> shader;
		std::unique_ptr<Renderer::Shader> simpleDepthShader;
		std::unique_ptr<Renderer::Shader> debugDepthQuad;
		std::unique_ptr<Renderer::Shader> debugCascadeShader;

		unsigned int planeVAO, planeVBO;
		unsigned int cubeVAO, cubeVBO;
		unsigned int quadVAO, quadVBO;
		unsigned int woodTexture;

		unsigned int lightFBO;
		unsigned int lightDepthMaps;
		unsigned int depthMapResolution;

		unsigned int matricesUBO;

		unsigned int m_Width, m_Height;

		int debugLayer;

		float cameraNearPlane;
		float cameraFarPlane;

		glm::vec3 lightDir;

		std::vector<unsigned int> visualizerVAOs;
		std::vector<unsigned int> visualizerVBOs;
		std::vector<unsigned int> visualizerEBOs;

	};
}