#include "SampleGeometryNormal.h"
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace sample
{

	SampleGeometryNormal::SampleGeometryNormal()
		: m_ClearColor{ 0.2f, 0.3f, 0.3f, 1.0f }, m_ShowNormal(true)
	{
		// load model
		m_Model = std::make_unique<Model>("res/models/nanosuit/nanosuit.obj");
		// build and compile shader
		m_Shader = std::make_unique<Renderer::Shader>("res/shaders/geometry/Default.vert",
			"res/shaders/geometry/Default.frag");
		m_ShaderNormal = std::make_unique<Renderer::Shader>("res/shaders/geometry/Normal.vert",
			"res/shaders/geometry/Normal.frag", "res/shaders/geometry/Normal.geom");
	}

	SampleGeometryNormal::~SampleGeometryNormal()
	{

	}

	void SampleGeometryNormal::OnUpdate(float deltaTime)
	{

	}

	void SampleGeometryNormal::OnRender(const Camera& camera)
	{
		glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// configure transformation matrices
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)m_Width / (float)m_Height,
			1.0f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, -1.0f));
		model = glm::scale(model, glm::vec3(0.16f, 0.16f, 0.16f));
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("projection", 1, projection);
		m_Shader->SetUniformMat4f("view", 1, view);
		m_Shader->SetUniformMat4f("model", 1, model);
		// draw model as usual
		m_Model->Draw(*m_Shader);

		// then draw model with normal visualizing geometry shader
		if (m_ShowNormal)
		{
			m_ShaderNormal->Bind();
			m_ShaderNormal->SetUniformMat4f("projection", 1, projection);
			m_ShaderNormal->SetUniformMat4f("view", 1, view);
			m_ShaderNormal->SetUniformMat4f("model", 1, model);

			m_Model->Draw(*m_ShaderNormal);
		}
	}

	void SampleGeometryNormal::OnImGuiRenderer()
	{
		ImGui::Checkbox("Show Normal", &m_ShowNormal);
		ImGui::ColorEdit4("Clear Color", m_ClearColor);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
			ImGui::GetIO().Framerate);
	}

	int SampleGeometryNormal::m_Width = 800;

	int SampleGeometryNormal::m_Height = 600;

}