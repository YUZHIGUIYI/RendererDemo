#include "SampleRenderModel.h"
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace sample
{
	int SampleRenderModel::m_Width = 800;
	int SampleRenderModel::m_Height = 600;

	SampleRenderModel::SampleRenderModel()
		: m_ClearColor{ 0.2f, 0.3f, 0.3f, 1.0f }
	{
		m_Shader = std::make_unique<Renderer::Shader>("res/shaders/Nanosuit.vert", "res/shaders/Nanosuit.frag");
		m_Model = std::make_unique<Model>("res/models/nanosuit/nanosuit.obj");

		// 点光源
		m_Shader->Bind();
		m_Shader->SetUniformVec3f("pointLights[0].position", glm::vec3(0.7f, 1.0f, 2.0f));
		m_Shader->SetUniformVec3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		m_Shader->SetUniformVec3f("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		m_Shader->SetUniformVec3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		m_Shader->SetUniform1f("pointLights[0].constant", 1.0f);
		m_Shader->SetUniform1f("pointLights[0].linear", 0.09f);
		m_Shader->SetUniform1f("pointLights[0].quadratic", 0.032f);

		m_Shader->SetUniformVec3f("pointLights[1].position", glm::vec3(2.3f, -3.3f, -2.0f));
		m_Shader->SetUniformVec3f("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		m_Shader->SetUniformVec3f("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		m_Shader->SetUniformVec3f("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		m_Shader->SetUniform1f("pointLights[1].constant", 1.0f);
		m_Shader->SetUniform1f("pointLights[1].linear", 0.09f);
		m_Shader->SetUniform1f("pointLights[1].quadratic", 0.032f);

		RD_INFO("Start SampleRenderModel");
	}

	SampleRenderModel::~SampleRenderModel()
	{
		RD_INFO("Exit SampleRenderModel");
	}

	void SampleRenderModel::OnUpdate(float deltaTime)
	{

	}

	void SampleRenderModel::OnRender(const Camera& camera)
	{
		// 清除颜色缓冲 | 深度缓冲
		glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_Shader->Bind();
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)m_Width / (float)m_Height,
			0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		m_Shader->SetUniformMat4f("projection", 1, projection);
		m_Shader->SetUniformMat4f("view", 1, view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		m_Shader->SetUniformMat4f("model", 1, model);
		m_Shader->SetUniformVec3f("viewPos", camera.GetPosition());
		m_Model->Draw(*m_Shader);
	}

	void SampleRenderModel::OnImGuiRenderer()
	{
		ImGui::ColorEdit4("Clear Color", m_ClearColor);
		ImGui::SliderInt("Near Width", &m_Width, 400, 1200);
		ImGui::SliderInt("Near Height", &m_Height, 300, 900);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
			ImGui::GetIO().Framerate);
	}

}