#include "RenderWindow.h"
#include <stb_image/stb_image.h>
#include "Log.h"
#include "Input.h"

float RenderWindow::m_DeltaTime = 0.0f;
float RenderWindow::m_LastFrame = 0.0f;
double RenderWindow::m_Lastx;
double RenderWindow::m_Lasty;
bool RenderWindow::m_FirstMouse = true;
bool RenderWindow::m_ShowMouse = true;
Camera RenderWindow::camera;
std::unique_ptr<RenderScene> RenderWindow::SceneBuffer;

GLenum RenderWindow::m_Mode;

RenderWindow::RenderWindow(int width, int height, const char* title, 
	GLFWmonitor* monitor /*= nullptr*/, GLFWwindow* share /*= nullptr*/)
	: m_Width(width), m_Height(height), m_WindowName(title),
	m_Window(nullptr), m_Monitor(monitor), m_Share(share)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_Window = glfwCreateWindow(width, height, title, monitor, share);
	if (m_Window == nullptr)
	{
		RD_CORE_ERROR("Failed to create GLFW window");
		glfwTerminate();
	}
	if (m_Window)
	{
		// set GLFW icon
		GLFWimage icon;
		icon.pixels = stbi_load("res/textures/RendererIcon.png", &icon.width, &icon.height, nullptr, 4);
		if (icon.pixels)
		{
			glfwSetWindowIcon(m_Window, 1, &icon);
			stbi_image_free(icon.pixels);
		}
		else
		{
			stbi_image_free(icon.pixels);
		}

		glfwMakeContextCurrent(m_Window);  // 通知GLFW将窗口的上下文设置为当前线程的主上下文
		glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);
		glfwSetCursorPosCallback(m_Window, mouse_callback);
		glfwSetKeyCallback(m_Window, key_callback);
		glfwSetScrollCallback(m_Window, scroll_callback);
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		// 调用任何OpenGL的函数之前我们需要初始化GLAD - load all OpenGL function pointers
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			RD_CORE_ERROR("Failed to initialize GLAD");
		}
		
		render_window_init(width, height);

		// Enable OpenGL Depth Test
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}
}

RenderWindow::~RenderWindow()
{
	if (m_Window)
	{
		glfwTerminate();
		RD_CORE_INFO("Exit Renderer Demo");
	}
}

float RenderWindow::GetDeltaTime()
{
	return m_DeltaTime;
}

int RenderWindow::RenderWindowShouldClose()
{
	return glfwWindowShouldClose(m_Window);
}

void RenderWindow::RenderWindowSwapBuffers()
{
	glfwSwapBuffers(m_Window);
}

void RenderWindow::RenderWindowPollEvents()
{
	glfwPollEvents();
}

GLFWwindow* RenderWindow::GetGLFWwindow()
{
	return m_Window;
}

void RenderWindow::Update()
{

	// Plot Mode
	/*if (glfwGetKey(m_Window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		if (m_Mode == GL_FILL)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			m_Mode = GL_LINE;
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			m_Mode = GL_FILL;
		}
	}*/

	// Adjust accordingly - based on personal hardware configuration
	float currentFrame = static_cast<float>(glfwGetTime());
	m_DeltaTime = currentFrame - m_LastFrame;
	m_LastFrame = currentFrame;
	Renderer::Input::DeltaTime = m_DeltaTime;
	// #TODO: Fix me
	// separate camera movement
	float cameraSpeed = 2.5f * m_DeltaTime;
	if (Renderer::Input::RendererMode == Renderer::RENDERERMODE::SAMPLE_MODE)
	{
		if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(CameraMovement::FORWARD, m_DeltaTime);
		if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(CameraMovement::BACKWARD, m_DeltaTime);
		if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(CameraMovement::LEFT, m_DeltaTime);
		if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(CameraMovement::RIGHT, m_DeltaTime);
	}

	// Clear color buffer | depth buffer | stencil buffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderWindow::render_window_init(int width, int height)
{
	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	m_Mode = GL_FILL;
	m_Lastx = static_cast<double>(width / 2.0);
	m_Lasty = static_cast<double>(height / 2.0);
	m_FirstMouse = true;
	SceneBuffer = std::make_unique<RenderScene>(width, height);
}

void RenderWindow::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
		return;
	}

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
	{
		Renderer::Input::RendererMode = Renderer::RENDERERMODE::SAMPLE_MODE;
		return;
	}

	if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
	{
		Renderer::Input::RendererMode = Renderer::RENDERERMODE::GAME_MODE;
		return;
	}

	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		if (m_ShowMouse)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			m_ShowMouse = false;
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_ShowMouse = true;
		}
	}
	
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			Renderer::Input::Keys[key] = true;
		else if (action == GLFW_RELEASE)
		{
			Renderer::Input::Keys[key] = false;
			Renderer::Input::KeysProcessed[key] = false;
		}
	}
}

void RenderWindow::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SceneBuffer->RescaleFrameBuffer(width, height);
}

void RenderWindow::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (Renderer::Input::RendererMode == Renderer::RENDERERMODE::GAME_MODE)
	{
		return;
	}

	if (m_FirstMouse)
	{
		m_Lastx = xpos;
		m_Lasty = ypos;
		m_FirstMouse = false;
	}

	float xOffset = xpos - m_Lastx;
	float yOffset = m_Lasty - ypos;
	m_Lastx = xpos;
	m_Lasty = ypos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void RenderWindow::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (Renderer::Input::RendererMode == Renderer::RENDERERMODE::GAME_MODE)
	{
		return;
	}
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

