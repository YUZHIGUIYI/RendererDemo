#include "ImGuiUIManager.h"
#include "Log.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

ImGuiUIManager::ImGuiUIManager(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	
	SetStyle();
	
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 400");
}

ImGuiUIManager::~ImGuiUIManager()
{

}

void ImGuiUIManager::Update(const Camera& camera)
{
	bool show = true;
	// start the ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuiIO& io = ImGui::GetIO();

	ShowDockSpace(show);

	UpdateWindows(camera);

	/*ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());*/
}

void ImGuiUIManager::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

}

void ImGuiUIManager::UpdateWindows(const Camera& camera)
{
	for (size_t i = 0; i < m_Windows.size(); ++i)
	{
		m_Windows[i]->Update(camera);
	}
}

void ImGuiUIManager::DeleteWindows()
{
	
}
