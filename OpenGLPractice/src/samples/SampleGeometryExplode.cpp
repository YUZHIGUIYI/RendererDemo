#include "SampleGeometryExplode.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace sample
{

	SampleGeometryExplode::SampleGeometryExplode()
		: m_ClearColor{ 0.2f, 0.3f, 0.3f, 1.0f }
	{
		// load model
		m_Model = std::make_unique<Model>("res/models/nanosuit/nanosuit.obj");
		// build and compile shader
		m_Shader = std::make_unique<Renderer::Shader>("res/shaders/geometry/Explode.vert", 
			"res/shaders/geometry/Explode.frag", "res/shaders/geometry/Explode.geom");
	}

	SampleGeometryExplode::~SampleGeometryExplode()
	{

	}

	void SampleGeometryExplode::OnUpdate(float deltaTime)
	{

	}

	void SampleGeometryExplode::OnRender(const Camera& camera, RenderScene* scenebuffer)
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
		// add time component to geometry shader in the form of a uniform
		m_Shader->SetUniform1f("time", static_cast<float>(glfwGetTime()));
		// draw model
		m_Model->Draw(*m_Shader);
	}

	void SampleGeometryExplode::OnImGuiRenderer()
	{
		ImGui::ColorEdit4("Clear Color", m_ClearColor);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
			ImGui::GetIO().Framerate);
	}

	int SampleGeometryExplode::m_Width = 800;

	int SampleGeometryExplode::m_Height = 600;

}