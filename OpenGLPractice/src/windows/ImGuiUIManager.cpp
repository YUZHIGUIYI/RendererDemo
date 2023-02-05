#include "ImGuiUIManager.h"
#include "Log.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

ImGuiUIManager::ImGuiUIManager(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;			// Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;			// Enable Multi-Viewport / Platform Windows

	ImGui::StyleColorsDark();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	float fontSize = 20.0f;
	io.Fonts->AddFontFromFileTTF("res/fonts/Roboto-Medium.ttf", fontSize);
	
	SetStyle();
	
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 400");
}

ImGuiUIManager::~ImGuiUIManager()
{
	// Clearup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiUIManager::Begin(const Camera& camera, RenderScene* scenebuffer)
{
	static bool show = true;
	static bool p_open = true;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	// start the ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Options"))
	{
		ImGui::MenuItem("Fullscreen", nullptr, &opt_fullscreen);
		ImGui::MenuItem("Padding", nullptr, &opt_padding);
		ImGui::Separator();

		if (ImGui::MenuItem("Close", nullptr, false))
			p_open = !p_open;
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	// ShowDemoWindow - Debug only
	ImGui::ShowDemoWindow(&show);

	// Show Windows component

	UpdateWindows(camera, scenebuffer);
}

void ImGuiUIManager::End()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere
	// Call glfwMakeContextCurrent(window) directly
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void ImGuiUIManager::AddWindow(std::unique_ptr<ImGuiWindowBase> newWin)
{
	m_Windows.push_back(std::move(newWin));
}

void ImGuiUIManager::ShowDockSpace(bool bopen)
{

}

void ImGuiUIManager::SetStyle()
{
	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

	// Headers
	colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Buttons
	colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}

void ImGuiUIManager::UpdateWindows(const Camera& camera, RenderScene* scenebuffer)
{
	static bool sceneshow = true;

	for (size_t i = 0; i < m_Windows.size(); ++i)
	{
		m_Windows[i]->Update(camera, scenebuffer);
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport", &sceneshow);
	{
		//ImGui::BeginChild("Render");
		//float width = ImGui::GetContentRegionAvail().x;
		//float height = ImGui::GetContentRegionAvail().y;

		ImGui::Image((ImTextureID)scenebuffer->GetFrameTexture(),
			ImGui::GetContentRegionAvail(),
			ImVec2(0, 1),
			ImVec2(1, 0));
		//ImGui::EndChild();
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void ImGuiUIManager::DeleteWindows()
{
	
}
