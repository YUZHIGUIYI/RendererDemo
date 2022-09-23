#include "SampleStencil.h"
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace sample
{
	int SampleStencil::m_Width = 800;

	int SampleStencil::m_Height = 600;

	SampleStencil::SampleStencil()
		: m_ClearColor{ 0.2f, 0.3f, 0.3f, 1.0f }
	{
		// 开启模板测试
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);  // 模板测试开启此项目
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		
		float cubeVertices[] = {
			// positions          // texture Coords
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};
		float planeVertices[] = {
			// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
			 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
			-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
			-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

			 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
			-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
			 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
		};

		// cube
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
		// plane
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
		// build and compile shaders
		m_ShaderOne = std::make_unique<Renderer::Shader>("res/shaders/stencil/StencilTest.vert",
			"res/shaders/stencil/StencilTest.frag");
		m_ShaderTwo = std::make_unique<Renderer::Shader>("res/shaders/stencil/StencilTest.vert",
			"res/shaders/stencil/StencilSingleColor.frag");
		// load textures
		m_TextureOne = std::make_unique<Renderer::Texture>("res/textures/marble.jpg");
		m_TextureTwo = std::make_unique<Renderer::Texture>("res/textures/metal.png");
		// shader configuration
		m_ShaderOne->Bind();
		//m_TextureOne->Bind(0);
		m_ShaderOne->SetUniform1i("texture1", 0);
	}

	SampleStencil::~SampleStencil()
	{
		// 关闭模板测试
		glDisable(GL_STENCIL_TEST);

		glDeleteVertexArrays(1, &cubeVAO);
		glDeleteVertexArrays(1, &planeVAO);
		glDeleteBuffers(1, &cubeVBO);
		glDeleteBuffers(1, &planeVBO);
	}

	void SampleStencil::OnUpdate(float deltaTime)
	{

	}

	void SampleStencil::OnRender(const Camera& camera)
	{
		glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// set uniform
		m_ShaderTwo->Bind();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)m_Width / (float)m_Height,
			0.1f, 100.0f);
		m_ShaderTwo->SetUniformMat4f("view", 1, view);
		m_ShaderTwo->SetUniformMat4f("projection", 1, projection);
		
		m_ShaderOne->Bind();
		m_ShaderOne->SetUniformMat4f("view", 1, view);
		m_ShaderOne->SetUniformMat4f("projection", 1, projection);

		// draw floor as normal, but don't write floor to the stencil buffer, we only care about the containers
		glStencilMask(0x00);
		// floor
		glBindVertexArray(planeVAO);
		m_TextureTwo->Bind(0);
		m_ShaderOne->SetUniformMat4f("model", 1, glm::mat4(1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		// first, render pass, draw objects as normal, writing to the stencil buffer
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		// cubes
		glBindVertexArray(cubeVAO);
		m_TextureOne->Bind(0);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		m_ShaderOne->SetUniformMat4f("model", 1, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		m_ShaderOne->SetUniformMat4f("model", 1, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// second, render pass: now draw slightly scaled version of the objects, this time disabling
		// stencil writings
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		m_ShaderTwo->Bind();
		float scale = 1.1f;
		// cubes
		glBindVertexArray(cubeVAO);
		m_TextureOne->Bind(0);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		model = glm::scale(model, glm::vec3(scale, scale, scale));
		m_ShaderTwo->SetUniformMat4f("model", 1, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(scale, scale, scale));
		m_ShaderTwo->SetUniformMat4f("model", 1, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);

	}

	void SampleStencil::OnImGuiRenderer()
	{
		ImGui::ColorEdit4("Clear Color", m_ClearColor);
		ImGui::SliderInt("Near Width", &m_Width, 400, 1200);
		ImGui::SliderInt("Near Height", &m_Height, 300, 900);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
			ImGui::GetIO().Framerate);
	}

}