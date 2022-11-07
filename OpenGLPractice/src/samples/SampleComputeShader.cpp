#include "SampleComputeShader.h"
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include "Input.h"
#include "Log.h"

namespace sample
{

	SampleComputeShader::SampleComputeShader()
		: m_Width(800), m_Height(600)
	{
		for (int idx = 0; idx < 3; ++idx)
		{
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
		}
		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);

		RD_INFO("maximum number of work groups in X dimension {0}", max_compute_work_group_count[0]);
		RD_INFO("maximum number of work groups in Y dimension {0}", max_compute_work_group_count[1]);
		RD_INFO("maximum number of work groups in Z dimension {0}", max_compute_work_group_count[2]);
		RD_INFO("maximum size of a work group in X dimension {0}", max_compute_work_group_size[0]);
		RD_INFO("maximum size of a work group in Y dimension {0}", max_compute_work_group_size[1]);
		RD_INFO("maximum size of a work group in Z dimension {0}", max_compute_work_group_size[2]);
		RD_INFO("Number of invocations in a single local work group that may be dispatched to a compute shader {0}", max_compute_work_group_invocations);

		screenQuad = std::make_unique<Renderer::Shader>("res/shaders/raytracing/ScreenQuad.vert",
			"res/shaders/raytracing/ScreenQuad.frag");
		computeShader = std::make_unique<Renderer::ComputeShader>("res/shaders/raytracing/RayTracing.comp");

		screenQuad->Bind();
		screenQuad->SetUniform1i("tex", 0);

		// create texture for opengl operation
		glGenTextures(1, &texture);

	}

	SampleComputeShader::~SampleComputeShader()
	{
		glDeleteVertexArrays(1, &quadVAO);
		glDeleteBuffers(1, &quadVBO);
		glDeleteTextures(1, &texture);
	}

	void SampleComputeShader::OnUpdate(float deltaTime)
	{

	}

	void SampleComputeShader::OnRender(const Camera& camera, RenderScene* scenebuffer)
	{
		if (!Done || scenebuffer->HasChanged())
		{
			Init(scenebuffer);
		}

		computeShader->Bind();
		computeShader->SetUniform1f("t", glfwGetTime());
		computeShader->SetUniform1f("width", (float)m_Width);
		glDispatchCompute((unsigned int)m_Width / 10, (unsigned int)m_Height / 10, 1);

		// make sure writing to image has finished before read
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// render image to quad
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		screenQuad->Bind();
		RenderQuad();

		scenebuffer->BlitFramebuffer();
	}

	void SampleComputeShader::OnImGuiRenderer()
	{
		ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.6f, 1.0f), "Compute Shader Example");
	}

	void SampleComputeShader::Init(RenderScene* scenebuffer)
	{
		m_Width = scenebuffer->GetWidth();
		m_Height = scenebuffer->GetHeight();
		Done = true;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);

		glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	void SampleComputeShader::RenderQuad()
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