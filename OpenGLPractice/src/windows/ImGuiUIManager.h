#pragma once

#include "ImGuiWindowBase.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

class ImGuiUIManager
{
public:
	ImGuiUIManager(GLFWwindow* window);
	~ImGuiUIManager();

	void Begin(const Camera& camera, RenderScene* scenebuffer);
	void End();
	void AddWindow(std::unique_ptr<ImGuiWindowBase> newWin);

private:
	std::vector<std::unique_ptr<ImGuiWindowBase>> m_Windows;

private:
	void ShowDockSpace(bool bopen);
	void SetStyle();
	void UpdateWindows(const Camera& camera, RenderScene* scenebuffer);
	void DeleteWindows();
};