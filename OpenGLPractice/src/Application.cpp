#include "windows/RenderWindow.h"
#include "windows/ImGuiUIManager.h"
#include "windows/SampleMenu.h"
#include "windows/ImGuiGameMenu.h"
#include "windows/ImGuiRayTracerMenu.h"
#include "Input.h"
#include "Log.h"

const unsigned int WIDTH = 1600;
const unsigned int HEIGHT = 1200;

int main()
{
	Renderer::Input::Init();
	Renderer::Log::Init();
	RD_CORE_INFO("Welcome to Renderer Demo");

	// Create render window
	RenderWindow renderwindow(WIDTH, HEIGHT, "Renderer Demo");

	// ImGui UI Manager
	ImGuiUIManager UIManager(renderwindow.GetGLFWwindow());

	// Add UI component
	std::unique_ptr<SampleMenu> sampleMenu = std::make_unique<SampleMenu>();
	UIManager.AddWindow(std::move(sampleMenu));

	std::unique_ptr<ImGuiGameMenu> gameMenu = std::make_unique<ImGuiGameMenu>();
	UIManager.AddWindow(std::move(gameMenu));

	std::unique_ptr<ImGuiRayTracerMenu> raytracerMenu = std::make_unique<ImGuiRayTracerMenu>();
	UIManager.AddWindow(std::move(raytracerMenu));

	// Render cycle
	while (!renderwindow.RenderWindowShouldClose())
	{
		renderwindow.Update();

		// Start ImGui frame rendering
		UIManager.Begin(RenderWindow::camera, renderwindow.GetSceneBuffer());

		// End ImGui rendering
		UIManager.End();

		renderwindow.RenderWindowSwapBuffers();
		renderwindow.RenderWindowPollEvents();
	}

	return 0;
}