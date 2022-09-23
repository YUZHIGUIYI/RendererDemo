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

	void Update(const Camera& camera);
	void Render();
	void AddWindow(std::unique_ptr<ImGuiWindowBase> newWin);

private:
	std::vector<std::unique_ptr<ImGuiWindowBase>> m_Windows;

private:
	void ShowDockSpace(bool bopen);
	void SetStyle();
	void UpdateWindows(const Camera& camera);
	void DeleteWindows();
};