#include "SampleAdvancedGLSL.h"
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sample
{

	SampleAdvancedGLSL::SampleAdvancedGLSL()
		: m_ClearColor{ 0.2f, 0.3f, 0.3f, 1.0f }
	{
		// set up vertices data
		float cubeVertices[] = {
			// positions         
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
		};
		// cube VAO
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		// build and compile shaders
		m_Shader[0] = std::make_unique<Renderer::Shader>("res/shaders/advancedglsl/AdvancedGLSL.vert",
			"res/shaders/advancedglsl/Red.frag");
		m_Shader[1] = std::make_unique<Renderer::Shader>("res/shaders/advancedglsl/AdvancedGLSL.vert",
			"res/shaders/advancedglsl/Green.frag");
		m_Shader[2] = std::make_unique<Renderer::Shader>("res/shaders/advancedglsl/AdvancedGLSL.vert",
			"res/shaders/advancedglsl/Blue.frag");
		m_Shader[3] = std::make_unique<Renderer::Shader>("res/shaders/advancedglsl/AdvancedGLSL.vert",
			"res/shaders/advancedglsl/Yellow.frag");
		// configure a uniform buffer object
		uniformBlockIndex[0] = glGetUniformBlockIndex(m_Shader[0]->GetRendererID(), "Matrices");
		uniformBlockIndex[1] = glGetUniformBlockIndex(m_Shader[1]->GetRendererID(), "Matrices");
		uniformBlockIndex[2] = glGetUniformBlockIndex(m_Shader[2]->GetRendererID(), "Matrices");
		uniformBlockIndex[3] = glGetUniformBlockIndex(m_Shader[3]->GetRendererID(), "Matrices");
		// then we link each shader's uniform block to this uniform binding point
		glUniformBlockBinding(m_Shader[0]->GetRendererID(), uniformBlockIndex[0], 0);
		glUniformBlockBinding(m_Shader[1]->GetRendererID(), uniformBlockIndex[1], 0);
		glUniformBlockBinding(m_Shader[2]->GetRendererID(), uniformBlockIndex[2], 0);
		glUniformBlockBinding(m_Shader[3]->GetRendererID(), uniformBlockIndex[3], 0);
		// now actually create the buffer
		glGenBuffers(1, &uboMatrices);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		// define the range of the buffer that links to a uniform binding point
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

		// store the projection matrix (we only do this once now)
		glm::mat4 projection = glm::perspective(45.0f, (float)m_Width / (float)m_Height,
			0.1f, 100.0f);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	SampleAdvancedGLSL::~SampleAdvancedGLSL()
	{
		glDeleteVertexArrays(1, &cubeVAO);
		glDeleteBuffers(1, &cubeVBO);
	}

	void SampleAdvancedGLSL::OnUpdate(float deltaTime)
	{

	}

	void SampleAdvancedGLSL::OnRender(const Camera& camera)
	{
		glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// set the view and projection matrix in the uniform block
		// we only have to do this once per loop iteration
		glm::mat4 view = camera.GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// draw four cubes
		// red
		glBindVertexArray(cubeVAO);
		m_Shader[0]->Bind();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f)); // move top-left
		m_Shader[0]->SetUniformMat4f("model", 1, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// green
		m_Shader[1]->Bind();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f)); // move top-right
		m_Shader[1]->SetUniformMat4f("model", 1, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// yellow
		m_Shader[3]->Bind();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f)); // move bottom-left
		m_Shader[3]->SetUniformMat4f("model", 1, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// blue
		m_Shader[2]->Bind();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f)); // move bottom-right
		m_Shader[2]->SetUniformMat4f("model", 1, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void SampleAdvancedGLSL::OnImGuiRenderer()
	{
		ImGui::ColorEdit4("Clear Color", m_ClearColor);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
			ImGui::GetIO().Framerate);
	}

	int SampleAdvancedGLSL::m_Width = 800;

	int SampleAdvancedGLSL::m_Height = 600;

}