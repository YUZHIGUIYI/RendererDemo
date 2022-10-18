#include "SampleGammaCorrection.h"
#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Log.h"

namespace sample
{

	SampleGammaCorrection::SampleGammaCorrection()
		: gammaEnabled(false)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// set up vertex data and configure vertex attributes
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
		m_TextureFloorID = loadTexture("res/textures/wood.png", false);
		m_TextureFloorGammaID = loadTexture("res/textures/wood.png", true);

		// shader compile and configuration
		m_Shader = std::make_unique<Renderer::Shader>("res/shaders/gammacorrect/GammaCorrect.vert",
			"res/shaders/gammacorrect/GammaCorrect.frag");
		m_Shader->Bind();
		m_Shader->SetUniform1i("floorTexture", 0);

		// lighting info
		lightPositions.reserve(5);
		lightPositions.push_back(glm::vec3(-3.0f, 0.0f, 0.0f));
		lightPositions.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
		lightPositions.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		lightPositions.push_back(glm::vec3(3.0f, 0.0f, 0.0f));
		lightColors.reserve(5);
		lightColors.push_back(glm::vec3(0.25f));
		lightColors.push_back(glm::vec3(0.50f));
		lightColors.push_back(glm::vec3(0.75f));
		lightColors.push_back(glm::vec3(1.00f));
	}

	SampleGammaCorrection::~SampleGammaCorrection()
	{
		glDeleteVertexArrays(1, &planeVAO);
		glDeleteBuffers(1, &planeVBO);
		glDeleteTextures(1, &m_TextureFloorID);
		glDeleteTextures(1, &m_TextureFloorGammaID);
	}

	void SampleGammaCorrection::OnUpdate(float deltaTime)
	{

	}

	void SampleGammaCorrection::OnRender(const Camera& camera, RenderScene* scenebuffer)
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
		// set light uniforms
		glUniform3fv(glGetUniformLocation(m_Shader->GetRendererID(), "lightPositions"), 4, &lightPositions[0][0]);
		glUniform3fv(glGetUniformLocation(m_Shader->GetRendererID(), "lightColors"), 4, &lightColors[0][0]);
		m_Shader->SetUniformVec3f("viewPos", camera.GetPosition());
		m_Shader->SetUniform1i("gamma", gammaEnabled);
		// floor
		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gammaEnabled ? m_TextureFloorGammaID : m_TextureFloorID);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void SampleGammaCorrection::OnImGuiRenderer()
	{
		ImGui::Checkbox("Enable Gamma Correction", &gammaEnabled);
	}

	unsigned int SampleGammaCorrection::loadTexture(const char* path, bool gammaCorrection)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum internalFormat;
			GLenum dataFormat;
			if (nrComponents == 1)
			{
				internalFormat = dataFormat = GL_RED;
			}
			else if (nrComponents == 3)
			{
				internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
				dataFormat = GL_RGB;
			}
			else if (nrComponents == 4)
			{
				internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
				dataFormat = GL_RGBA;
			}

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			RD_ERROR("Texture failed to load at path: {0}", path);
			stbi_image_free(data);
		}

		return textureID;
	}

	int SampleGammaCorrection::m_Width = 800;

	int SampleGammaCorrection::m_Height = 600;

}