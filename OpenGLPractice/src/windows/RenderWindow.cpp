#include "RenderWindow.h"
#include "Log.h"

double RenderWindow::m_Lastx;
double RenderWindow::m_Lasty;
bool RenderWindow::m_FirstMouse = true;
Camera RenderWindow::camera;
GLenum RenderWindow::m_Mode;

RenderWindow::RenderWindow(int width, int height, const char* title, 
	GLFWmonitor* monitor /*= nullptr*/, GLFWwindow* share /*= nullptr*/)
	: m_Width(width), m_Height(height), m_WindowName(title),
	m_DeltaTime(0.0), m_LastFrame(0.0),
	m_Window(nullptr), m_Monitor(monitor), m_Share(share)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_Window = glfwCreateWindow(width, height, title, monitor, share);
	if (m_Window == nullptr)
	{
		RD_CORE_ERROR("Failed to create GLFW window");
		glfwTerminate();
	}
	if (m_Window)
	{
		glfwMakeContextCurrent(m_Window);  // 通知GLFW将窗口的上下文设置为当前线程的主上下文
		glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);
		glfwSetCursorPosCallback(m_Window, mouse_callback);
		glfwSetScrollCallback(m_Window, scroll_callback);
		//glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		// 调用任何OpenGL的函数之前我们需要初始化GLAD - load all OpenGL function pointers
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			RD_CORE_ERROR("Failed to initialize GLAD");
		}
		
		render_window_init(width, height);
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

void RenderWindow::ProcessInput()
{
	if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(m_Window, true);
	}
	else if (glfwGetKey(m_Window, GLFW_KEY_TAB) == GLFW_PRESS)
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
	}

	// Adjust accordingly - based on personal hardware configuration
	float currentFrame = static_cast<float>(glfwGetTime());
	m_DeltaTime = currentFrame - m_LastFrame;
	m_LastFrame = currentFrame;
	float cameraSpeed = 2.5f * m_DeltaTime;
	if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraMovement::FORWARD, m_DeltaTime);
	if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraMovement::BACKWARD, m_DeltaTime);
	if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraMovement::LEFT, m_DeltaTime);
	if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraMovement::RIGHT, m_DeltaTime);
}

void RenderWindow::render_window_init(int width, int height)
{
	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	m_Mode = GL_FILL;
	m_Lastx = static_cast<double>(width / 2.0);
	m_Lasty = static_cast<double>(height / 2.0);
	m_FirstMouse = true;
}

void RenderWindow::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void RenderWindow::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

