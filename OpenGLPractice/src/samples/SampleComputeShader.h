#pragma once

#include "Sample.h"
#include "Shader.h"
#include "raytracing/ComputeShader.h"

namespace sample
{
	class SampleComputeShader : public Sample
	{
	public:
		SampleComputeShader();
		~SampleComputeShader();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

		void Init(RenderScene* scenebuffer);

	private:
		void RenderQuad();

	private:
		unsigned int quadVAO{ 0 };
		unsigned int quadVBO{ 0 };
		bool Done{ false };

		unsigned int texture;
		int m_Width{ 800 };
		int m_Height{ 600 };

		int max_compute_work_group_count[3];
		int max_compute_work_group_size[3];
		int max_compute_work_group_invocations;

		std::unique_ptr<Renderer::Shader> screenQuad;
		std::unique_ptr<Renderer::ComputeShader> computeShader;

		
	};
}