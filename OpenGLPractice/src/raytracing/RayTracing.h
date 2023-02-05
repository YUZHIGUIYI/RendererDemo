#pragma once

#include <memory>
#include "Camera.h"
#include "RenderScene.h"
#include "ComputeShader.h"
#include "Shader.h"

namespace RayTracer
{
	class RayTracing
	{
	private:
		float m_AspectRatio;
		glm::vec2 m_Resolution;
		int m_NumFrames;

		unsigned int quadVAO, quadVBO;
		unsigned int rayTexture;
		unsigned int Width, Height;
		bool Done;

		std::unique_ptr<Renderer::Shader> screenShader;
		std::unique_ptr<Renderer::ComputeShader> computeShader;
	
	public:
		RayTracing();
		~RayTracing();

		void Init();
		void Update(float dt);
		void Render(const Camera& camera, RenderScene* scenebuffer);

	private:
		void ReInit(RenderScene* scenebuffer);
		void RenderQuad();
	};
}