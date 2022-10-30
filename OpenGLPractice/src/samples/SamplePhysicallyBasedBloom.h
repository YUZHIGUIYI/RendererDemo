#pragma once

#include "Sample.h"
#include "bloom/BloomRenderer.h"

namespace sample
{
	class SamplePhysicallyBasedBloom : public Sample
	{
	public:
		SamplePhysicallyBasedBloom();
		~SamplePhysicallyBasedBloom();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

		void Init(RenderScene* scenebuffer);

	private:
		void RenderCube();
		unsigned int LoadTexture(const char* path, bool gammaCorrection);

		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Renderer::Shader> m_ShaderLight;
		std::unique_ptr<Renderer::Shader> m_ShaderBlur;
		std::unique_ptr<Renderer::Shader> m_ShaderBloomFinal;

		std::unique_ptr<BloomRenderer> bloomRenderer;

		unsigned int m_Width, m_Height;
		unsigned int cubeVAO, cubeVBO;

		bool EnableBloom{false};
		bool Done{ false };

		int ProgramChoice{ 1 };
		float BloomFilterRadius{ 0.005f };
		float Exposure{ 1.0f };

		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightColors;

		unsigned int woodTexture, containerTexture;

		unsigned int hdrFBO;
		unsigned int colorBuffers[2];
		unsigned int rboDepth;

		unsigned int pingpongFBO[2];
		unsigned int pingpongColorbuffers[2];
	};
}