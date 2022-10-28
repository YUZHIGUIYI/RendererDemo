#pragma once

#include "Sample.h"
#include "Shader.h"
#include "Model.h"
#include "Animator.h"
#include <memory>

namespace sample 
{
	class SampleSkeletalAnimation : public Sample
	{
	public:
		SampleSkeletalAnimation();
		~SampleSkeletalAnimation();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

		void Init(RenderScene* scenebuffer);

	private:
		bool Done;

		std::unique_ptr<Renderer::Shader> m_Shader;
		std::unique_ptr<Model> m_Model;
		std::unique_ptr<Animation> danceAnimation;
		std::unique_ptr<Animator> m_Animator;

		static unsigned int m_Width;
		static unsigned int m_Height;
	};
}