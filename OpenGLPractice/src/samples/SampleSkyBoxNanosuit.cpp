#include "SampleSkyBoxNanosuit.h"
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image/stb_image.h>
#include "Log.h"

namespace sample
{

	SampleSkyBoxNanosuit::SampleSkyBoxNanosuit()
		: Done(false)
	{
		// model
		m_Model = std::make_unique<Model>("res/models/nanosuit_reflection/nanosuit.obj");
		// skybox vertices
		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
		// skybox VAO
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		// load textures - skybox
		m_Faces.reserve(6);
		m_Faces.push_back("res/textures/skybox/right.jpg");
		m_Faces.push_back("res/textures/skybox/left.jpg");
		m_Faces.push_back("res/textures/skybox/top.jpg");
		m_Faces.push_back("res/textures/skybox/bottom.jpg");
		m_Faces.push_back("res/textures/skybox/front.jpg");
		m_Faces.push_back("res/textures/skybox/back.jpg");
		// texture ID of skybox
		m_CubeMapID = LoadCubeMap();
		// build and compile shaders
		m_ShaderModel = std::make_unique<Renderer::Shader>("res/shaders/skybox/Nanosuit.vert",
			"res/shaders/skybox/Nanosuit.frag");
		m_ShaderSkyBox = std::make_unique<Renderer::Shader>("res/shaders/skybox/SkyBox.vert",
			"res/shaders/skybox/SkyBox.frag");
		// shader configuration - skybox
		m_ShaderSkyBox->Bind();
		m_ShaderSkyBox->SetUniform1i("skybox", 0);
		// shader configuration - model
		// firstly, point lights
		m_ShaderModel->Bind();
		m_ShaderModel->SetUniformVec3f("pointLights[0].position", glm::vec3(0.7f, 1.0f, 2.0f));
		m_ShaderModel->SetUniformVec3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		m_ShaderModel->SetUniformVec3f("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		m_ShaderModel->SetUniformVec3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		m_ShaderModel->SetUniform1f("pointLights[0].constant", 1.0f);
		m_ShaderModel->SetUniform1f("pointLights[0].linear", 0.09f);
		m_ShaderModel->SetUniform1f("pointLights[0].quadratic", 0.032f);

		m_ShaderModel->SetUniformVec3f("pointLights[1].position", glm::vec3(2.3f, -3.3f, -2.0f));
		m_ShaderModel->SetUniformVec3f("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		m_ShaderModel->SetUniformVec3f("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		m_ShaderModel->SetUniformVec3f("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		m_ShaderModel->SetUniform1f("pointLights[1].constant", 1.0f);
		m_ShaderModel->SetUniform1f("pointLights[1].linear", 0.09f);
		m_ShaderModel->SetUniform1f("pointLights[1].quadratic", 0.032f);
		// secondly, skybox texture - ensure the texture element ID exceed three
		m_ShaderModel->SetUniform1i("skybox", 10);
		// end
		RD_INFO("Start Render SkyBox Model");
	}

	SampleSkyBoxNanosuit::~SampleSkyBoxNanosuit()
	{
		glDeleteVertexArrays(1, &skyboxVAO);
		glDeleteBuffers(1, &skyboxVBO);
		glDeleteTextures(1, &m_CubeMapID);
	}

	void SampleSkyBoxNanosuit::OnUpdate(float deltaTime)
	{

	}

	void SampleSkyBoxNanosuit::OnRender(const Camera& camera, RenderScene* scenebuffer)
	{
		if (!Done || scenebuffer->HasChanged())
		{
			Init(scenebuffer);
		}
		// write into post processing framebuffer, and then transfer texture id to imgui image
		scenebuffer->Bind();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// draw model as first - remember to bind skybox texture to reflect or refract
		m_ShaderModel->Bind();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)m_Width / (float)m_Height,
			0.1f, 100.0f);
		m_ShaderModel->SetUniformMat4f("model", 1, model);
		m_ShaderModel->SetUniformMat4f("view", 1, view);
		m_ShaderModel->SetUniformMat4f("projection", 1, projection);
		m_ShaderModel->SetUniformVec3f("viewPos", camera.GetPosition());
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapID);
		m_Model->Draw(*m_ShaderModel);

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		m_ShaderSkyBox->Bind();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));  // remove translation from the view matrix
		m_ShaderSkyBox->SetUniformMat4f("view", 1, view);
		m_ShaderSkyBox->SetUniformMat4f("projection", 1, projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapID);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		scenebuffer->Unbind();
	}

	void SampleSkyBoxNanosuit::OnImGuiRenderer()
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
			ImGui::GetIO().Framerate);
	}

	void SampleSkyBoxNanosuit::Init(RenderScene* scenebuffer)
	{
		m_Width = scenebuffer->GetWidth();
		m_Height = scenebuffer->GetHeight();
		Done = true;
	}

	unsigned int SampleSkyBoxNanosuit::LoadCubeMap()
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		stbi_set_flip_vertically_on_load(false);
		int width, height, nrChannels;
		for (unsigned int i = 0; i < m_Faces.size(); i++)
		{
			unsigned char* data = stbi_load(m_Faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				GLenum format;
				if (nrChannels == 1) format = GL_RED;
				else if (nrChannels == 3) format = GL_RGB;
				else if (nrChannels == 4) format = GL_RGBA;
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				RD_ERROR("CubeMap Texture failed to load at path: {0}", m_Faces[i]);
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

	int SampleSkyBoxNanosuit::m_Width = 800;

	int SampleSkyBoxNanosuit::m_Height = 600;

}