#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include "Model.h"
#include <memory>
#include <vector>
#include <string>

namespace sample
{
	class SampleSkyBoxNanosuit : public Sample
	{
	public:
		SampleSkyBoxNanosuit();
		~SampleSkyBoxNanosuit();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

		void Init(RenderScene* scenebuffer);

	private:
		unsigned int LoadCubeMap();

	private:
		bool Done;

		unsigned int skyboxVAO, skyboxVBO;

		unsigned int m_CubeMapID;

		std::unique_ptr<Renderer::Shader> m_ShaderModel;
		std::unique_ptr<Renderer::Shader> m_ShaderSkyBox;
		std::unique_ptr<Model> m_Model;

		std::vector<std::string> m_Faces;
		static int m_Width;
		static int m_Height;
	};
}