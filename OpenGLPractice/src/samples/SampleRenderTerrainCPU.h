#pragma once

#include "Sample.h"
#include "Shader.h"

namespace sample
{
	class SampleRenderTerrainCPU : public Sample
	{
	public:
		SampleRenderTerrainCPU();
		~SampleRenderTerrainCPU();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

		void Init(RenderScene* scenebuffer);

	private:
		std::unique_ptr<Renderer::Shader> MapShader;

		unsigned int terrainVAO, terrainVBO, terrainIBO;

		unsigned int m_Width, m_Height;

		unsigned int numStrips, numTrisPerStrip;

		bool Done, displayGrayScale, useWireFrame;
	};
}
