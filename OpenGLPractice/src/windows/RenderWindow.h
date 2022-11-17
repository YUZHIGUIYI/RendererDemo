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
	void CheckCameraMovement();
	void UpdateCamera();

private:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void render_window_init(int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

	static void key_pressed(int key);
	static void key_released(int key);

private:
	unsigned int m_Width, m_Height;
	std::string m_WindowName;
	
	GLFWwindow* m_Window;
	GLFWmonitor* m_Monitor;
	GLFWwindow* m_Share;

	static float m_DeltaTime, m_LastFrame;
	static double m_Currentx, m_Currenty;
	static double m_Lastx, m_Lasty;
	static double m_Deltax, m_Deltay;
	static bool m_ShowMouse;
	static bool m_FirstMouse;
	static GLenum m_Mode;
	
	static bool m_CameraMoved;
	static bool m_MouseLook;

	static float m_HeadingSpeed;
	static float m_SideWaysSpeed;
	static float m_CameraSensitivity;
	static float m_CameraSpeed;

public:
	static Camera camera;
	static std::unique_ptr<RenderScene> SceneBuffer;
};