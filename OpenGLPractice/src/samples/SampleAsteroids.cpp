#include "SampleAsteroids.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace sample
{

	SampleAsteroids::SampleAsteroids()
		: radius(150.0f), offset(25.0f)
	{
		// build and compile shaders
		asteroidShader = std::make_unique<Renderer::Shader>("res/shaders/instances/Asteroids.vert",
			"res/shaders/instances/Asteroids.frag");
		planetShader = std::make_unique<Renderer::Shader>("res/shaders/instances/Planet.vert",
			"res/shaders/instances/Planet.frag");
		// load modules
		rock = std::make_unique<Model>("res/models/rock/rock.obj");
		planet = std::make_unique<Model>("res/models/planet/planet.obj");
		// generate a large list of semi-random model transformation matrices
		modelMatrices = new glm::mat4[amount];
		srand(static_cast<unsigned int>(glfwGetTime()));
		for (unsigned int i = 0; i < amount; ++i)
		{
			glm::mat4 model = glm::mat4(1.0f);
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)amount * 360.0f;
			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement;
			model = glm::translate(model, glm::vec3(x, y, z));

			// 2. scale: Scale between 0.05 and 0.25f
			float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
			model = glm::scale(model, glm::vec3(scale));

			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = static_cast<float>((rand() % 360));
			model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			// 4. now add to list of matrices
			modelMatrices[i] = model;
		}
		
		// configure instanced array
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

		// set transformation matrices as an instance vertex attribute (with divisor 1)
		unsigned int size = rock->GetMeshesSize();
		const auto& Meshes = rock->GetMeshes();
		for (unsigned int i = 0; i < size; i++)
		{
			unsigned int VAO = Meshes[i].GetVertexArray();
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}

	}

	SampleAsteroids::~SampleAsteroids()
	{
		delete[] modelMatrices;
		glDeleteBuffers(1, &buffer);
	}

	void SampleAsteroids::OnUpdate(float deltaTime)
	{
		
	}

	void SampleAsteroids::OnRender(const Camera& camera, RenderScene* scenebuffer)
	{
		scenebuffer->Bind();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// configure transformation matrices
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)m_Width / (float)m_Height,
			0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		asteroidShader->Bind();
		asteroidShader->SetUniformMat4f("projection", 1, projection);
		asteroidShader->SetUniformMat4f("view", 1, view);
		planetShader->Bind();
		planetShader->SetUniformMat4f("projection", 1, projection);
		planetShader->SetUniformMat4f("view", 1, view);

		// draw planet
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		planetShader->SetUniformMat4f("model", 1, model);
		planet->Draw(*planetShader);

		// draw meteorites
		asteroidShader->Bind();
		asteroidShader->SetUniform1i("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rock->GetFirstTextureId());
		unsigned int size = rock->GetMeshesSize();
		const auto& Meshes = rock->GetMeshes();
		for (unsigned int i = 0; i < size; ++i)
		{
			glBindVertexArray(Meshes[i].GetVertexArray());
			glDrawElementsInstanced(GL_TRIANGLES, Meshes[i].GetIndicesSize(), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}

		scenebuffer->Unbind();
	}

	void SampleAsteroids::OnImGuiRenderer()
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
			ImGui::GetIO().Framerate);
	}

	unsigned int SampleAsteroids::amount = 10000;

	int SampleAsteroids::m_Width = 800;

	int SampleAsteroids::m_Height = 600;

}