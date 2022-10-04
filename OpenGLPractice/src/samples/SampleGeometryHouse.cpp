#include "SampleGeometryHouse.h"
#include <glad/glad.h>
#include <imgui/imgui.h>

namespace sample
{

	SampleGeometryHouse::SampleGeometryHouse()
	{
		// set up vertex data
		float points[] = {
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
		};
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);
		// build and compile shaders
		m_Shader = std::make_unique<Renderer::Shader>("res/shaders/geometry/GeometryHouse.vert",
			"res/shaders/geometry/GeometryHouse.frag", "res/shaders/geometry/GeometryHouse.geom");

	}

	SampleGeometryHouse::~SampleGeometryHouse()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	void SampleGeometryHouse::OnUpdate(float deltaTime)
	{

	}

	void SampleGeometryHouse::OnRender(const Camera& camera)
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// draw points
		m_Shader->Bind();
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 4);
	}

	void SampleGeometryHouse::OnImGuiRenderer()
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
			ImGui::GetIO().Framerate);
	}

}