#include "RayTracing.h"

namespace RayTracer
{

	RayTracing::RayTracing()
		: quadVAO(0), quadVBO(0), Width(800), Height(600), Done(false)
	{
	}

	RayTracing::~RayTracing()
	{
		glDeleteVertexArrays(1, &quadVAO);
		glDeleteBuffers(1, &quadVBO);

		glDeleteTextures(1, &rayTexture);
	}

	void RayTracing::Init()
	{
		screenShader = std::make_unique<Renderer::Shader>("res/shaders/raytracing/ScreenQuad.vert",
			"res/shaders/raytracing/ScreenQuad.frag");
		computeShader = std::make_unique<Renderer::ComputeShader>("res/shaders/raytracing/PathTracing.comp");

		screenShader->Bind();
		screenShader->SetUniform1i("tex", 0);
		// create texture
		glGenTextures(1, &rayTexture);
	}

	void RayTracing::Update(float dt)
	{

	}

	void RayTracing::Render(const Camera& camera, RenderScene* scenebuffer)
	{
		if (!Done || scenebuffer->HasChanged())
		{
			ReInit(scenebuffer);
		}

		computeShader->Bind();

		if (camera.m_CameraMoved) m_NumFrames = 0;

		computeShader->SetUniform1i("u_NumFrames", m_NumFrames);
		computeShader->SetUniform1f("u_Accum", float(m_NumFrames) / float(m_NumFrames + 1));
		computeShader->SetUniform1f("u_FOV", glm::radians(camera.GetZoom()));
		computeShader->SetUniform1f("u_AspectRatio", m_AspectRatio);
		computeShader->SetUniformVec2f("u_Resolution", m_Resolution);
		glm::mat4 InvViewMat = glm::inverse(camera.GetViewMatrix());
		glm::mat4 InvProjectionMat = glm::inverse(camera.GetProjection());
		computeShader->SetUniformMat4f("u_InvViewMat", 1, InvViewMat);
		computeShader->SetUniformMat4f("u_InvProjectionMat", 1, InvProjectionMat);

		glDispatchCompute((unsigned int)Width, (unsigned int)Height, 1);
		// make sure writing to image has finished before read
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// render image to quad
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		screenShader->Bind();
		RenderQuad();

		scenebuffer->BlitFramebuffer();

		++m_NumFrames;
	}

	void RayTracing::ReInit(RenderScene* scenebuffer)
	{
		Width = scenebuffer->GetWidth();
		Height = scenebuffer->GetHeight();
		Done = true;

		m_AspectRatio = float(Width) / float(Height);
		m_Resolution = glm::vec2(Width, Height);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rayTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Width, Height, 0, GL_RGBA, GL_FLOAT, nullptr);

		glBindImageTexture(0, rayTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rayTexture);
	}

	void RayTracing::RenderQuad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] =
			{
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

}