#pragma once

#include "Sample.h"
#include "ShaderN.h"

namespace sample
{
	class SampleRenderTerrainGPU : public Sample
	{
	public:
		SampleRenderTerrainGPU();
		~SampleRenderTerrainGPU();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

		void Init(RenderScene* scenebuffer);

	private:
		std::unique_ptr<Renderer::ShaderN> MapShader;

		unsigned int terrainVAO, terrainVBO;
		unsigned int texture;
		unsigned int NUM_PATCH_PTS;

		unsigned rez;

		unsigned int m_Width, m_Height;

		bool Done, displayGrayScale, useWireFrame;
	};
}