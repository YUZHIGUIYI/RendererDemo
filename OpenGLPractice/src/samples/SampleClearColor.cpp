#include "SampleClearColor.h"
#include <glad/glad.h>
#include <imgui/imgui.h>

namespace sample
{

	SampleClearColor::SampleClearColor()
		: m_ClearColor{ 0.2f, 0.3f, 0.3f, 1.0f }
	{
	}

	SampleClearColor::~SampleClearColor()
	{
	}

	void SampleClearColor::OnUpdate(float deltaTime)
	{
		
	}

	void SampleClearColor::OnRender(const Camera& camera, RenderScene* scenebuffer)
	{
		glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void SampleClearColor::OnImGuiRenderer()
	{
		ImGui::ColorEdit4("Clear Color", m_ClearColor);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, 
			ImGui::GetIO().Framerate);
	}

}