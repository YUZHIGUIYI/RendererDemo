#include "SampleSkeletalAnimation.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>
#include "Input.h"

namespace sample
{

	SampleSkeletalAnimation::SampleSkeletalAnimation()
		: Done(false)
	{
		stbi_set_flip_vertically_on_load(true);

		m_Shader = std::make_unique<Renderer::Shader>("res/shaders/anim/AnimModel.vert",
			"res/shaders/anim/AnimModel.frag");
		m_Model = std::make_unique<Model>("res/models/vampire/dancing_vampire.dae");
		danceAnimation = std::make_unique<Animation>("res/models/vampire/dancing_vampire.dae", m_Model.get());
		m_Animator = std::make_unique<Animator>(danceAnimation.get());
	}

	SampleSkeletalAnimation::~SampleSkeletalAnimation()
	{

	}

	void SampleSkeletalAnimation::OnUpdate(float deltaTime)
	{

	}

	void SampleSkeletalAnimation::OnRender(const Camera& camera, RenderScene* scenebuffer)
	{
		if (!Done || scenebuffer->HasChanged())
		{
			Init(scenebuffer);
		}
		// write into postprocessing framebuffer
		scenebuffer->Bind();
		// input
		m_Animator->UpdateAnimation(Renderer::Input::DeltaTime);
		// render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// set uniforms
		m_Shader->Bind();
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)m_Width / (float)m_Height,
			0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		m_Shader->SetUniformMat4f("projection", 1, projection);
		m_Shader->SetUniformMat4f("view", 1, view);

		auto transforms = m_Animator->GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
		{
			m_Shader->SetUniformMat4f("finalBonesMatrices[" + std::to_string(i) + "]", 1, transforms[i]);
		}

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		m_Shader->SetUniformMat4f("model", 1, model);
		m_Model->Draw(*m_Shader);

		scenebuffer->Unbind();
	}

	void SampleSkeletalAnimation::OnImGuiRenderer()
	{
		ImGui::TextColored(ImVec4(0.5f, 0.8f, 0.9f, 1.0f), "Skeletal Animation");
	}

	void SampleSkeletalAnimation::Init(RenderScene* scenebuffer)
	{
		m_Width = scenebuffer->GetWidth();
		m_Height = scenebuffer->GetHeight();
		Done = true;
	}

	unsigned int SampleSkeletalAnimation::m_Width = 800;

	unsigned int SampleSkeletalAnimation::m_Height = 600;

}