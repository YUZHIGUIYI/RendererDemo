#include "ImGuiWindowBase.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

ImGuiWindowBase::ImGuiWindowBase()
	: bshow(true)
{
}

void ImGuiWindowBase::Update(const Camera& camera, RenderScene* scenebuffer)
{

}

void ImGuiWindowBase::Show()
{

}

bool ImGuiWindowBase::Begin(const std::string& name)
{
	return ImGui::Begin(name.c_str(), &bshow);
}

void ImGuiWindowBase::End()
{
	ImGui::End();
}
