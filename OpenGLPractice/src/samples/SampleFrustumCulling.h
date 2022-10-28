#pragma once

#include "Sample.h"
#include "Shader.h"

class Model;
class Entity;

namespace sample
{
	class SampleFrustumCulling : public Sample
	{
	public:
		SampleFrustumCulling();
		~SampleFrustumCulling();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

		void Init(RenderScene* scenebuffer);

	private:
		std::unique_ptr<Entity> ourEntity;
		std::unique_ptr<Model> model;
		std::unique_ptr<Renderer::Shader> ourShader;

		unsigned int m_Width;
		unsigned int m_Height;

		bool Done;
	};
}