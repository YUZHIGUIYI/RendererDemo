#include "SamplePhysicallyBasedBloom.h"
#include <imgui/imgui.h>
#include <stb_image/stb_image.h>

namespace sample
{

	SamplePhysicallyBasedBloom::SamplePhysicallyBasedBloom()
		: cubeVAO(0), cubeVBO(0)
	{
		m_Shader = std::make_unique<Renderer::Shader>("res/shaders/phybloom/Bloom.vert", "res/shaders/phybloom/Bloom.frag");
		m_ShaderLight = std::make_unique<Renderer::Shader>("res/shaders/phybloom/Bloom.vert", "res/shaders/phybloom/LightBox.frag");
		m_ShaderBlur = std::make_unique<Renderer::Shader>("res/shaders/phybloom/Blur.vert", "res/shaders/phybloom/Blur.frag");
		m_ShaderBloomFinal = std::make_unique<Renderer::Shader>("res/shaders/phybloom/BloomFinal.vert", "res/shaders/phybloom/BloomFinal.frag");

		bloomRenderer = std::make_unique<BloomRenderer>();

		woodTexture = LoadTexture("res/textures/wood.png", true);  // SRGB texture
		containerTexture = LoadTexture("res/textures/container2.png", true);  // SRGB texture

		// configure (floating point) framebuffers
		glGenFramebuffers(1, &hdrFBO);
		// create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
		glGenTextures(2, colorBuffers);
		// create and attach depth buffer (renderbuffer)
		glGenRenderbuffers(1, &rboDepth);
		// ping-pong-framebuffer for blurring
		glGenFramebuffers(2, pingpongFBO);
		glGenTextures(2, pingpongColorbuffers);
		// lighting info
		// positions
		lightPositions.reserve(4);
		lightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
		lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
		lightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
		lightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));
		// colors
		lightPositions.reserve(4);
		lightColors.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
		lightColors.push_back(glm::vec3(10.0f, 0.0f, 0.0f));
		lightColors.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
		lightColors.push_back(glm::vec3(0.0f, 5.0f, 0.0f));

	}

	SamplePhysicallyBasedBloom::~SamplePhysicallyBasedBloom()
	{
		bloomRenderer->Destroy();
		
		glDeleteFramebuffers(1, &hdrFBO);
		glDeleteRenderbuffers(1, &rboDepth);
		glDeleteFramebuffers(2, pingpongFBO);

		glDeleteTextures(2, colorBuffers);
		glDeleteTextures(2, pingpongColorbuffers);
		glDeleteTextures(1, &woodTexture);
		glDeleteTextures(1, &containerTexture);

		glDeleteVertexArrays(1, &cubeVAO);
		glDeleteBuffers(1, &cubeVBO);

	}

	void SamplePhysicallyBasedBloom::OnUpdate(float deltaTime)
	{

	}

	void SamplePhysicallyBasedBloom::OnRender(const Camera& camera, RenderScene* scenebuffer)
	{
		if (!Done || scenebuffer->HasChanged())
		{
			Init(scenebuffer);
		}

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. render scene into floating point framebuffer
		// -----------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)m_Width / (float)m_Height, 
			0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("projection", 1, projection);
		m_Shader->SetUniformMat4f("view", 1, view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		// set lighting uniforms
		for (unsigned int i = 0; i < lightPositions.size(); ++i)
		{
			m_Shader->SetUniformVec3f("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			m_Shader->SetUniformVec3f("lights[" + std::to_string(i) + "].Color", lightColors[i]);
		}
		m_Shader->SetUniformVec3f("viewPos", camera.GetPosition());
		// create one large cube that acts as the floor
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
		model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
		m_Shader->SetUniformMat4f("model", 1, model);
		RenderCube();
		// then create multiple cubes as the scenery
		glBindTexture(GL_TEXTURE_2D, containerTexture);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		m_Shader->SetUniformMat4f("model", 1, model);
		RenderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.5f));
		m_Shader->SetUniformMat4f("model", 1, model);
		RenderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		m_Shader->SetUniformMat4f("model", 1, model);
		RenderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
		model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(1.25));
		m_Shader->SetUniformMat4f("model", 1, model);
		RenderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
		model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		m_Shader->SetUniformMat4f("model", 1, model);
		RenderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		m_Shader->SetUniformMat4f("model", 1, model);
		RenderCube();

		// finally show all the light sources as bright cubes
		m_ShaderLight->Bind();
		m_ShaderLight->SetUniformMat4f("projection", 1, projection);
		m_ShaderLight->SetUniformMat4f("view", 1, view);

		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(lightPositions[i]));
			model = glm::scale(model, glm::vec3(0.25f));
			m_ShaderLight->SetUniformMat4f("model", 1, model);
			m_ShaderLight->SetUniformVec3f("lightColor", lightColors[i]);
			RenderCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		EnableBloom = (ProgramChoice == 1) ? false : true;
		bool horizontal = true;

		// 2.A) bloom is disabled
		if (ProgramChoice == 1) {}
		// 2.B) blur bright fragments with two-pass Gaussian Blur
		else if (ProgramChoice == 2)
		{
			bool first_iteration = true;
			unsigned int amount = 10;
			m_ShaderBlur->Bind();
			for (unsigned int i = 0; i < amount; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
				m_ShaderBlur->SetUniform1i("horizontal", horizontal);
				glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
				bloomRenderer->RenderQuad();
				horizontal = !horizontal;
				if (first_iteration)
					first_iteration = false;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		// 2.C) use unthresholded bloom with progressive downsample/upsampling
		else if (ProgramChoice == 3)
		{
			bloomRenderer->RenderBloomTexture(colorBuffers[1], BloomFilterRadius);
		}

		// 3. now render floating point color buffer to 2D quad and 
		// tonemap HDR colors to default framebuffer's (clamped) color range
		scenebuffer->Bind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_ShaderBloomFinal->Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		if (ProgramChoice == 1) {
			glBindTexture(GL_TEXTURE_2D, 0); // trick to bind invalid texture "0", we don't care either way!
		}
		if (ProgramChoice == 2) {
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
		}
		else if (ProgramChoice == 3) {
			glBindTexture(GL_TEXTURE_2D, bloomRenderer->BloomTexture());
		}

		m_ShaderBloomFinal->SetUniform1i("programChoice", ProgramChoice);
		m_ShaderBloomFinal->SetUniform1f("exposure", Exposure);
		bloomRenderer->RenderQuad();

		scenebuffer->Unbind();
	}

	void SamplePhysicallyBasedBloom::OnImGuiRenderer()
	{
		ImGui::TextColored(ImVec4(0.1f, 0.5f, 0.9f, 1.0f), "Physically Based Bloom");
		ImGui::SliderInt("Program Choice", &ProgramChoice, 1, 3);
		ImGui::SliderFloat("Bloom Filter Radius(Enable Choice 3)", &BloomFilterRadius, 0.005f, 0.02f);
		ImGui::SliderFloat("Exposure(Enable Bloom)", &Exposure, 1.0f, 5.0f);
	}

	void SamplePhysicallyBasedBloom::Init(RenderScene* scenebuffer)
	{
		m_Width = scenebuffer->GetWidth();
		m_Height = scenebuffer->GetHeight();
		Done = true;

		// configure (floating point) framebuffers
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		// create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// attach texture to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
		}
		// create and attach depth buffer (renderbuffer)
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			RD_ERROR("Framebuffer not complete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// ping-pong-framebuffer for blurring
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
			// also check if framebuffers are complete (no need for depth buffer)
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				RD_ERROR("Framebuffer not complete!");
		}

		// shader configuration
		// --------------------
		m_Shader->Bind();
		m_Shader->SetUniform1i("diffuseTexture", 0);
		m_ShaderBlur->Bind();
		m_ShaderBlur->SetUniform1i("image", 0);
		m_ShaderBloomFinal->Bind();
		m_ShaderBloomFinal->SetUniform1i("scene", 0);
		m_ShaderBloomFinal->SetUniform1i("bloomBlur", 1);

		// bloom renderer
		// --------------
		bloomRenderer->Init(m_Width, m_Height);
	}

	void SamplePhysicallyBasedBloom::RenderCube()
	{
		// initialize (if necessary)
		if (cubeVAO == 0)
		{
			float vertices[] = {
				// back face
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				// right face
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
				 // bottom face
				 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
				  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				 // top face
				 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
				  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
			};
			glGenVertexArrays(1, &cubeVAO);
			glGenBuffers(1, &cubeVBO);
			// fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// link vertex attributes
			glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	unsigned int SamplePhysicallyBasedBloom::LoadTexture(const char* path, bool gammaCorrection)
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

}