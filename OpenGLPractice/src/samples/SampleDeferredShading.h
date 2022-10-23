#pragma once

#include "Sample.h"
#include "Shader.h"
#include "Model.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace sample
{
	class SampleDeferredShading : public Sample
	{
	public:
		SampleDeferredShading();
		~SampleDeferredShading();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

		void Init(RenderScene* scenebuffer);

	private:
		void renderCube();
		void renderQuad();

	private:
		bool Done;

		unsigned int cubeVAO, cubeVBO;
		unsigned int quadVAO, quadVBO;

		unsigned int gBuffer;
		unsigned int gPosition, gNormal, gAlbedoSpec;
		unsigned int attachments[3];
		unsigned int rboDepth;

		std::unique_ptr<Renderer::Shader> m_ShaderGeometryPass;
		std::unique_ptr<Renderer::Shader> m_ShaderLightingPass;
		std::unique_ptr<Renderer::Shader> m_ShaderLightBox;

		std::unique_ptr<Model> m_Model;

		std::vector<glm::vec3> objectPositions;
		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightColors;

		static unsigned int m_Width;
		static unsigned int m_Height;
	};
}