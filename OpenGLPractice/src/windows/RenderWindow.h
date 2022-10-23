#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include "Camera.h"
#include "RenderScene.h"

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
	void Update();

	RenderScene* GetSceneBuffer() const { return SceneBuffer.get(); };

private:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void render_window_init(int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
	unsigned int m_Width, m_Height;
	std::string m_WindowName;
	
	GLFWwindow* m_Window;
	GLFWmonitor* m_Monitor;
	GLFWwindow* m_Share;

	static float m_DeltaTime, m_LastFrame;
	static double m_Lastx, m_Lasty;
	static bool m_ShowMouse;
	static bool m_FirstMouse;
	static GLenum m_Mode;
public:
	static Camera camera;
	static std::unique_ptr<RenderScene> SceneBuffer;
};