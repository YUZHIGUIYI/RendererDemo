#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace sample
{
	class SampleBlending : public Sample
	{
	public:
		SampleBlending();
		~SampleBlending();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

	private:
		unsigned int cubeVAO, cubeVBO;
		unsigned int planeVAO, planeVBO;
		unsigned int transparentVAO, transparentVBO;

		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Renderer::Texture> m_TextureCube;
		std::unique_ptr<Renderer::Texture> m_TextureFloor;
		std::unique_ptr<Renderer::Texture> m_TextureTransparent;

		std::vector<glm::vec3> vegetation;

		float m_ClearColor[4];

		static int m_Width;
		static int m_Height;
	};
}