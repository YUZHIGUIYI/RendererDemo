#include "SampleLightingTextured.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Log.h"

namespace sample
{

	SampleLightingTextured::SampleLightingTextured()
		: sphereVAO(0), indexCount(0), nrRows(7), nrColumns(7), spacing(2.5f)
	{
		// build and compile shaders
		m_Shader = std::make_unique<Renderer::Shader>("res/shaders/pbrlighting/PBR.vert",
			"res/shaders/pbrlighting/PBR.frag");
		m_Shader->Bind();
		m_Shader->SetUniform1i("albedoMap", 0);
		m_Shader->SetUniform1i("normalMap", 1);
		m_Shader->SetUniform1i("metallicMap", 2);
		m_Shader->SetUniform1i("roughnessMap", 3);
		m_Shader->SetUniform1i("aoMap", 4);

		// load PBR material textures
		m_Albedo = std::make_unique<Renderer::Texture>("res/textures/pbr/rusted_iron/albedo.png");
		m_Normal = std::make_unique<Renderer::Texture>("res/textures/pbr/rusted_iron/normal.png");
		m_Metallic = std::make_unique<Renderer::Texture>("res/textures/pbr/rusted_iron/metallic.png");
		m_Roughness = std::make_unique<Renderer::Texture>("res/textures/pbr/rusted_iron/roughness.png");
		m_Ao = std::make_unique<Renderer::Texture>("res/textures/pbr/rusted_iron/ao.png");

		// lights
		lightPositions.reserve(5);
		lightPositions.push_back(glm::vec3(0.0f, 0.0f, 10.0f));
		lightColors.reserve(5);
		lightColors.push_back(glm::vec3(150.0f, 150.0f, 150.0f));
	}

	SampleLightingTextured::~SampleLightingTextured()
	{

	}

	void SampleLightingTextured::OnUpdate(float deltaTime)
	{

	}

	void SampleLightingTextured::OnRender(const Camera& camera, RenderScene* scenebuffer)
	{
		glViewport(0, 0, m_Width, m_Height);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_Shader->Bind();
		projection = glm::perspective(glm::radians(camera.GetZoom()), (float)m_Width / (float)m_Height,
			0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		m_Shader->SetUniformMat4f("projection", 1, projection);
		m_Shader->SetUniformMat4f("view", 1, view);
		m_Shader->SetUniformVec3f("camPos", camera.GetPosition());

		m_Albedo->Bind(0);
		m_Normal->Bind(1);
		m_Metallic->Bind(2);
		m_Roughness->Bind(3);
		m_Ao->Bind(4);

		// render rows * column number of spheres with material properties defined by textures
		// they all have the same material properties
		glm::mat4 model = glm::mat4(1.0f);
		for (int row = 0; row < nrRows; ++row)
		{
			for (int col = 0; col < nrColumns; ++col)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(
					(float)(col - (nrColumns / 2)) * spacing,
					(float)(row - (nrRows / 2)) * spacing,
					0.0f
				));
				m_Shader->SetUniformMat4f("model", 1, model);
				renderSphere();
			}
		}

		// render light source (simply re-render sphere at light positions)
		// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
		// keeps the codeprint small.
		for (unsigned int i = 0; i < 1; ++i)
		{
			glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
			newPos = lightPositions[i];
			m_Shader->SetUniformVec3f("lightPositions[" + std::to_string(i) + "]", newPos);
			m_Shader->SetUniformVec3f("lightColors[" + std::to_string(i) + "]", lightColors[i]);

			model = glm::mat4(1.0f);
			model = glm::translate(model, newPos);
			model = glm::scale(model, glm::vec3(0.5f));
			m_Shader->SetUniformMat4f("model", 1, model);
			renderSphere();
		}
	}

	void SampleLightingTextured::OnImGuiRenderer()
	{
		ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.5f, 1.0f), "PBR Lighting Textured");
	}

	void SampleLightingTextured::renderSphere()
	{
		if (sphereVAO == 0)
		{
			glGenVertexArrays(1, &sphereVAO);

			unsigned int vbo, ebo;
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);

			std::vector<glm::vec3> positions;
			std::vector<glm::vec2> uv;
			std::vector<glm::vec3> normals;
			std::vector<unsigned int> indices;

			const unsigned int X_SEGMENTS = 64;
			const unsigned int Y_SEGMENTS = 64;
			const float PI = 3.14159265359f;

			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
				{
					float xSegment = (float)x / (float)X_SEGMENTS;
					float ySegment = (float)y / (float)Y_SEGMENTS;
					float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
					float yPos = std::cos(ySegment * PI);
					float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

					positions.push_back(glm::vec3(xPos, yPos, zPos));
					uv.push_back(glm::vec2(xSegment, ySegment));
					normals.push_back(glm::vec3(xPos, yPos, zPos));
				}
			}

			bool oddRow = false;
			for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
			{
				if (!oddRow) // even rows: y == 0, y == 2; and so on
				{
					for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
					{
						indices.push_back(y * (X_SEGMENTS + 1) + x);
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					}
				}
				else
				{
					for (int x = X_SEGMENTS; x >= 0; --x)
					{
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
						indices.push_back(y * (X_SEGMENTS + 1) + x);
					}
				}
				oddRow = !oddRow;
			}
			indexCount = static_cast<unsigned int>(indices.size());

			std::vector<float> data;
			for (unsigned int i = 0; i < positions.size(); ++i)
			{
				data.push_back(positions[i].x);
				data.push_back(positions[i].y);
				data.push_back(positions[i].z);
				if (normals.size() > 0)
				{
					data.push_back(normals[i].x);
					data.push_back(normals[i].y);
					data.push_back(normals[i].z);
				}
				if (uv.size() > 0)
				{
					data.push_back(uv[i].x);
					data.push_back(uv[i].y);
				}
			}

			glBindVertexArray(sphereVAO);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			unsigned int stride = (3 + 2 + 3) * sizeof(float);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
		}

		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
	}

	unsigned int SampleLightingTextured::m_Width = 1280;

	unsigned int SampleLightingTextured::m_Height = 720;

}