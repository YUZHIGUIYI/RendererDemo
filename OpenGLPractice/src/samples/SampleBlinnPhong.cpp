#include "SampleBlinnPhong.h"
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sample
{

	SampleBlinnPhong::SampleBlinnPhong()
		: lightPos(0.0f, 0.0f, 0.0f), selectBlinnPhong(false)
	{
		// set up vertex data, buffer, and configure vertex attributes
		float planeVertices[] = {
			// positions            // normals         // texcoords
			 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
			-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
			-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

			 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
			-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
			 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
		};
		// plane VAO
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);
		// load textures
		m_Texture = std::make_unique<Renderer::Texture>("res/textures/wood.png");
		// build shader and configure shader
		m_Shader = std::make_unique<Renderer::Shader>("res/shaders/blinnphong/BlinnPhong.vert",
			"res/shaders/blinnphong/BlinnPhong.frag");
		m_Shader->Bind();
		m_Shader->SetUniform1i("floorTexture", 0);
	}

	SampleBlinnPhong::~SampleBlinnPhong()
	{
		glDeleteVertexArrays(1, &planeVAO);
		glDeleteBuffers(1, &planeVBO);
	}

	void SampleBlinnPhong::OnUpdate(float deltaTime)
	{

	}

	void SampleBlinnPhong::OnRender(const Camera& camera)
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// draw objects
		m_Shader->Bind();
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)m_Width / (float)m_Height, 
			0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		m_Shader->SetUniformMat4f("projection", 1, projection);
		m_Shader->SetUniformMat4f("view", 1, view);
		// set light uniform
		m_Shader->SetUniformVec3f("viewPos", camera.GetPosition());
		m_Shader->SetUniformVec3f("lightPos", lightPos);
		m_Shader->SetUniform1i("blinn", selectBlinnPhong);
		// floor
		glBindVertexArray(planeVAO);
		m_Texture->Bind(0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void SampleBlinnPhong::OnImGuiRenderer()
	{
		ImGui::Checkbox("Blinn-Phong", &selectBlinnPhong);
	}

	int SampleBlinnPhong::m_Width = 800;

	int SampleBlinnPhong::m_Height = 600;

}