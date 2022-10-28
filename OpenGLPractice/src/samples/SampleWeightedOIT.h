#pragma once

#include "Sample.h"
#include "Shader.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace sample
{
	class SampleWeightedOIT : public Sample
	{
	public:
		SampleWeightedOIT();
		~SampleWeightedOIT();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

		void Init(RenderScene* scenebuffer);

	private:
		glm::mat4 calculate_model_matrix(const glm::vec3& position, const glm::vec3& rotation,
			const glm::vec3& scale);

	private:
		bool Done;

		std::unique_ptr<Renderer::Shader> solidShader;
		std::unique_ptr<Renderer::Shader> transparentShader;
		std::unique_ptr<Renderer::Shader> compositeShader;
		std::unique_ptr<Renderer::Shader> screenShader;

		unsigned int quadVAO, quadVBO;
		unsigned int opaqueFBO, transparentFBO;
		unsigned int opaqueTexture, depthTexture;

		unsigned int accumTexture, revealTexture;
		
		glm::mat4 redModelMat;
		glm::mat4 greenModelMat;
		glm::mat4 blueModelMat;

		glm::vec4 zeroFillerVec;
		glm::vec4 oneFillerVec;

		static unsigned int m_Width, m_Height;
	};
}