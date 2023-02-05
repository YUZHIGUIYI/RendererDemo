#include "ImGuiRayTracerMenu.h"
#include <imgui/imgui.h>
#include "Log.h"
#include "Input.h"


ImGuiRayTracerMenu::ImGuiRayTracerMenu()
	: rayTracingSample(nullptr)
{
}

ImGuiRayTracerMenu::~ImGuiRayTracerMenu()
{
}

void ImGuiRayTracerMenu::Update(const Camera& camera, RenderScene* scenebuffer)
{
	if (bshow)
	{
		if (!Begin("Ray Tracer"))
		{
			End();
		}
		else
		{
			if (rayTracingSample != nullptr)
			{
				rayTracingSample->Update(Renderer::Input::DeltaTime);
				rayTracingSample->Render(camera, scenebuffer);
			}
			else
			{
				if (ImGui::Button("Enter RayTracer"))
				{
					rayTracingSample = std::make_unique<RayTracer::RayTracing>();
					rayTracingSample->Init();
					Renderer::Input::RendererMode = Renderer::RENDERERMODE::RENDER_MODE;
					RD_WARN("Start Ray Tracer");
				}
			}
			if (ImGui::Button("Exit Ray Tracer"))
			{
				if (rayTracingSample != nullptr)
				{
					rayTracingSample.reset();
					Renderer::Input::RendererMode = Renderer::RENDERERMODE::SAMPLE_MODE;
					RD_WARN("Exit Ray Tracer");
				}
			}
			End();
		}
	}
}
