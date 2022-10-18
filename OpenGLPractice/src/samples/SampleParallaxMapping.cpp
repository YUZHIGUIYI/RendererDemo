#include "SampleParallaxMapping.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sample
{

	SampleParallaxMapping::SampleParallaxMapping()
		: lightPos(0.5f, 1.0f, 0.3f), quadVAO(0), quadVBO(0), heightScale(0.1f)
	{
		// load textures
		m_TextureDiffuseMap = std::make_unique<Renderer::Texture>("res/textures/bricks2.jpg");
		m_TextureNormalMap = std::make_unique<Renderer::Texture>("res/textures/bricks2_normal.jpg");
		m_TextureHeightMap = std::make_unique<Renderer::Texture>("res/textures/bricks2_disp.jpg");
		// build, compile and configure shader
		m_Shader = std::make_unique<Renderer::Shader>("res/shaders/parallaxmapping/ParallaxMapping.vert",
			"res/shaders/parallaxmapping/ParallaxMapping.frag");
		m_Shader->Bind();
		m_Shader->SetUniform1i("diffuseMap", 0);
		m_Shader->SetUniform1i("normalMap", 1);
		m_Shader->SetUniform1i("depthMap", 2);
	}

	SampleParallaxMapping::~SampleParallaxMapping()
	{
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
	}

	void SampleParallaxMapping::OnUpdate(float deltaTime)
	{

	}

	void SampleParallaxMapping::OnRender(const Camera& camera, RenderScene* scenebuffer)
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// configure view/projection matrices
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)m_Width / (float)m_Height,
			0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("projection", 1, projection);
		m_Shader->SetUniformMat4f("view", 1, view);
		// render parallax-mapped quad
		auto time = static_cast<float>(glfwGetTime());
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(time * -10.0f),
			glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
		m_Shader->SetUniformMat4f("model", 1, model);
		m_Shader->SetUniformVec3f("viewPos", camera.GetPosition());
		m_Shader->SetUniformVec3f("lightPos", lightPos);
		m_Shader->SetUniform1f("heightScale", heightScale);  // can be adjust

		m_TextureDiffuseMap->Bind(0);
		m_TextureNormalMap->Bind(1);
		m_TextureHeightMap->Bind(2);

		renderQuad();

		// render light source (simply re-renders a smaller plane at the light's position for debugging/visualization)
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.1f));
		m_Shader->SetUniformMat4f("model", 1, model);
		renderQuad();
	}

	void SampleParallaxMapping::OnImGuiRenderer()
	{
		ImGui::TextColored(ImVec4(0.4f, 0.5f, 0.7f, 1.0f), "Parallax Mapping");
		ImGui::SliderFloat("Height Scale", &heightScale, 0.05f, 0.95f);
	}

	void SampleParallaxMapping::renderQuad()
	{
        if (quadVAO == 0)
        {
            // positions
            glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
            glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
            glm::vec3 pos3(1.0f, -1.0f, 0.0f);
            glm::vec3 pos4(1.0f, 1.0f, 0.0f);
            // texture coordinates
            glm::vec2 uv1(0.0f, 1.0f);
            glm::vec2 uv2(0.0f, 0.0f);
            glm::vec2 uv3(1.0f, 0.0f);
            glm::vec2 uv4(1.0f, 1.0f);
            // normal vector
            glm::vec3 nm(0.0f, 0.0f, 1.0f);

            // calculate tangent/bitangent vectors of both triangles
            glm::vec3 tangent1, bitangent1;
            glm::vec3 tangent2, bitangent2;
            // triangle 1
            // ----------
            glm::vec3 edge1 = pos2 - pos1;
            glm::vec3 edge2 = pos3 - pos1;
            glm::vec2 deltaUV1 = uv2 - uv1;
            glm::vec2 deltaUV2 = uv3 - uv1;

            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            tangent1 = glm::normalize(tangent1);

            bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
            bitangent1 = glm::normalize(bitangent1);

            // triangle 2
            // ----------
            edge1 = pos3 - pos1;
            edge2 = pos4 - pos1;
            deltaUV1 = uv3 - uv1;
            deltaUV2 = uv4 - uv1;

            f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            tangent2 = glm::normalize(tangent2);


            bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
            bitangent2 = glm::normalize(bitangent2);


            float quadVertices[] = {
                // positions            // normal         // texcoords  // tangent                          // bitangent
                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
            };
            // configure plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
	}

	unsigned int SampleParallaxMapping::m_Width = 800;

	unsigned int SampleParallaxMapping::m_Height = 600;

}