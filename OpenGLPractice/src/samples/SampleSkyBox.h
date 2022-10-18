#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <vector>
#include <string>

namespace sample
{
	class SampleSkyBox : public Sample
	{
	public:
		SampleSkyBox();
		~SampleSkyBox();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

	private:
		unsigned int LoadCubeMap();

	private:
		unsigned int cubeVAO, cubeVBO;
		unsigned int skyboxVAO, skyboxVBO;

		unsigned int m_CubeMapID;

		std::unique_ptr<Renderer::Shader> m_ShaderCube;
		std::unique_ptr<Renderer::Shader> m_ShaderSkyBox;

		std::unique_ptr<Renderer::Texture> m_TextureCube;

		std::vector<std::string> m_Faces;
		static int m_Width;
		static int m_Height;
	};
}