#pragma once

#include "Sample.h"
#include "Texture.h"
#include "Shader.h"
#include "Model.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace sample
{
	class SampleAsteroids : public Sample
	{
	public:
		SampleAsteroids();
		~SampleAsteroids();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera) override;
		void OnImGuiRenderer() override;

	private:
		unsigned int buffer;
		float radius;
		float offset;

		std::unique_ptr<Model> rock;
		std::unique_ptr<Model> planet;
		std::unique_ptr<Renderer::Shader> asteroidShader;
		std::unique_ptr<Renderer::Shader> planetShader;

		glm::mat4* modelMatrices;

		static unsigned int amount;
		static int m_Width;
		static int m_Height;
	};
}