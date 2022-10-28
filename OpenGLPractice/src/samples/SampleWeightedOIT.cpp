#include "SampleWeightedOIT.h"
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Log.h"

namespace sample
{

	SampleWeightedOIT::SampleWeightedOIT()
		: Done(false)
	{
		solidShader = std::make_unique<Renderer::Shader>("res/shaders/oit/Solid.vert",
			"res/shaders/oit/Solid.frag");
		transparentShader = std::make_unique<Renderer::Shader>("res/shaders/oit/Transparent.vert",
			"res/shaders/oit/Transparent.frag");
		compositeShader = std::make_unique<Renderer::Shader>("res/shaders/oit/Composite.vert",
			"res/shaders/oit/Composite.frag");
		screenShader = std::make_unique<Renderer::Shader>("res/shaders/oit/Screen.vert",
			"res/shaders/oit/Screen.frag");

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);

		glGenFramebuffers(1, &opaqueFBO);
		glGenFramebuffers(1, &transparentFBO);

		glGenTextures(1, &opaqueTexture);
		glGenTextures(1, &depthTexture);

		glGenTextures(1, &accumTexture);
		glGenTextures(1, &revealTexture);

		// set up transformation matrices
		redModelMat = calculate_model_matrix(glm::vec3(0.0f, 0.0f, 1.0f), 
			glm::vec3(0.0f), glm::vec3(1.0f));
		greenModelMat = calculate_model_matrix(glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f), glm::vec3(1.0f));
		blueModelMat = calculate_model_matrix(glm::vec3(0.0f, 0.0f, 2.0f),
			glm::vec3(0.0f), glm::vec3(1.0f));

		// set up intermediate variables
		zeroFillerVec = glm::vec4(0.0f);
		oneFillerVec = glm::vec4(1.0f);
	}

	SampleWeightedOIT::~SampleWeightedOIT()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDisable(GL_BLEND);

		glDeleteVertexArrays(1, &quadVAO);
		glDeleteBuffers(1, &quadVBO);

		glDeleteFramebuffers(1, &opaqueFBO);
		glDeleteFramebuffers(1, &transparentFBO);

		glDeleteTextures(1, &opaqueTexture);
		glDeleteTextures(1, &depthTexture);
		glDeleteTextures(1, &accumTexture);
		glDeleteTextures(1, &revealTexture);
	}

	void SampleWeightedOIT::OnUpdate(float deltaTime)
	{

	}

	void SampleWeightedOIT::OnRender(const Camera& camera, RenderScene* scenebuffer)
	{
		if (!Done || scenebuffer->HasChanged())
		{
			Init(scenebuffer);
		}

		// camera matrices
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)m_Width / (float)m_Height,
			0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 vp = projection * view;

		// draw solid objects (solid pass)

		// configure render states
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		// bind opaque framebuffer to render solid objects
		glBindFramebuffer(GL_FRAMEBUFFER, opaqueFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw red quad - opaque
		solidShader->Bind();
		solidShader->SetUniformMat4f("mvp", 1, vp * redModelMat);
		solidShader->SetUniformVec3f("color", glm::vec3(1.0, 0.0f, 0.0f));
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// draw transparent objects (transparent pass)

		// configure render states
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunci(0, GL_ONE, GL_ONE);
		glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
		glBlendEquation(GL_FUNC_ADD);

		// bind transparent framebuffer to render transparent objects
		glBindFramebuffer(GL_FRAMEBUFFER, transparentFBO);
		glClearBufferfv(GL_COLOR, 0, &zeroFillerVec[0]);
		glClearBufferfv(GL_COLOR, 1, &oneFillerVec[0]);

		// draw green quad
		transparentShader->Bind();
		transparentShader->SetUniformMat4f("mvp", 1, vp * greenModelMat);
		transparentShader->SetUniformVec4f("color", glm::vec4(0.0f, 1.0f, 0.0f, 0.5f));
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// draw blue quad
		transparentShader->SetUniformMat4f("mvp", 1, vp * blueModelMat);
		transparentShader->SetUniformVec4f("color", glm::vec4(0.0f, 0.0f, 1.0f, 0.5f));
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// draw composite image (composite pass)

		// set render states
		glDepthFunc(GL_ALWAYS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// bind opaque framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, opaqueFBO);

		// draw screen quad
		compositeShader->Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, accumTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, revealTexture);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// draw to backbuffer (final pass)
		// set render states
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
		glDisable(GL_BLEND);

		// bind backbuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// draw final screen quad
		screenShader->Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, opaqueTexture);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		scenebuffer->BlitFramebuffer();
	}

	void SampleWeightedOIT::OnImGuiRenderer()
	{
		ImGui::TextColored(ImVec4(0.1f, 0.8f, 0.6f, 1.0f), 
			"Order-independent transparency via Weighted Blended");
	}

	void SampleWeightedOIT::Init(RenderScene* scenebuffer)
	{
		m_Width = scenebuffer->GetWidth();
		m_Height = scenebuffer->GetHeight();
		Done = true;

		// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------
		float quadVertices[] = {
			// positions		// uv
			-1.0f, -1.0f, 0.0f,	0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,

			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f
		};

		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);

		// set up attachments for opaque framebuffer
		glBindTexture(GL_TEXTURE_2D, opaqueTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, opaqueFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, opaqueTexture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			RD_ERROR("Error::FrameBuffer::Opaque framebuffer is not complete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// set up attachments for transparent framebuffer
		glBindTexture(GL_TEXTURE_2D, accumTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindTexture(GL_TEXTURE_2D, revealTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_Width, m_Height, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, transparentFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTexture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, revealTexture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0); // opaque framebuffer's depth texture
	
		const GLenum transparentDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, transparentDrawBuffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			RD_ERROR("Error::FrameBuffer:: Transparent framebuffer is not complete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	glm::mat4 SampleWeightedOIT::calculate_model_matrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	{
		glm::mat4 trans = glm::mat4(1.0f);

		trans = glm::translate(trans, position);
		trans = glm::rotate(trans, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
		trans = glm::rotate(trans, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
		trans = glm::rotate(trans, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));
		trans = glm::scale(trans, scale);

		return trans;
	}

	unsigned int SampleWeightedOIT::m_Width = 800;

	unsigned int SampleWeightedOIT::m_Height = 600;

}