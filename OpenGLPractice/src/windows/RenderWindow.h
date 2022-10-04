#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Camera.h"

class RenderWindow
{
public:
	RenderWindow(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
	~RenderWindow();

	float GetDeltaTime();
	int RenderWindowShouldClose();
	void RenderWindowSwapBuffers();
	void RenderWindowPollEvents();
	GLFWwindow* GetGLFWwindow();
	void ProcessInput();

private:
	static void render_window_init(int width, int height);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
	unsigned int m_Width, m_Height;
	float m_DeltaTime, m_LastFrame;
	std::string m_WindowName;
	
	GLFWwindow* m_Window;
	GLFWmonitor* m_Monitor;
	GLFWwindow* m_Share;

	static double m_Lastx, m_Lasty;
	static bool m_ShowMouse;
	static bool m_FirstMouse;
	static GLenum m_Mode;
public:
	static Camera camera;
};