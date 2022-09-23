#include "windows/RenderWindow.h"
#include "windows/ImGuiUIManager.h"
#include "windows/SampleMenu.h"
#include "samples/SampleClearColor.h"
#include "samples/SampleRenderModel.h"
#include "samples/SampleStencil.h"
#include "samples/SampleBlending.h"
#include "samples/SampleBlendingWindow.h"
#include "samples/SampleFaceCulling.h"
#include "samples/SampleFrameBuffers.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Log.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int main()
{
	Renderer::Log::Init();
	RD_CORE_INFO("Welcome to Renderer Demo");

	// Create render window
	RenderWindow renderwindow(WIDTH, HEIGHT, "Renderer Demo");

	// Enable OpenGL Depth Test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// Enable Stencil Test
	//glEnable(GL_STENCIL_TEST);
	////glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// ImGui UI Manager
	ImGuiUIManager UIManager(renderwindow.GetGLFWwindow());

	// Add UI component
	std::unique_ptr<SampleMenu> sampleMenu = std::make_unique<SampleMenu>();
	sampleMenu->RegisterSample<sample::SampleClearColor>("Clear Color");
	sampleMenu->RegisterSample<sample::SampleRenderModel>("Render Nano-suit");
	sampleMenu->RegisterSample<sample::SampleStencil>("Stencil Testing");
	sampleMenu->RegisterSample<sample::SampleBlending>("Blending Grass");
	sampleMenu->RegisterSample<sample::SampleBlendingWindow>("Blending Window");
	sampleMenu->RegisterSample<sample::SampleFaceCulling>("Face Culling");
	sampleMenu->RegisterSample<sample::SampleFrameBuffers>("Frame Buffer");
	UIManager.AddWindow(std::move(sampleMenu));

	// Render cycle
	while (!renderwindow.RenderWindowShouldClose())
	{
		renderwindow.ProcessInput();

		// Clear color buffer | depth buffer | stencil buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Start ImGui frame
		UIManager.Update(RenderWindow::camera);
		
		float deltaTime = renderwindow.GetDeltaTime();

		// ImGui rendering
		UIManager.Render();

		renderwindow.RenderWindowSwapBuffers();
		renderwindow.RenderWindowPollEvents();
	}

	return 0;
}