#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;
double lastX = WIDTH / 2.0f;
double lastY = HEIGHT / 2.0f;
bool firstMouse = true;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 ligthPos(1.2f, 1.0f, 2.0f);

int count = 0;
float mixValue = 0.2f;
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);  // 通知GLFW将窗口的上下文设置为当前线程的主上下文
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// 捕捉鼠标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 调用任何OpenGL的函数之前我们需要初始化GLAD - load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 开启深度测试
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// 在y轴上翻转加载的纹理
	stbi_set_flip_vertically_on_load(true);

	// Shader类
	Shader shader("res/shaders/Nanosuit.vert", "res/shaders/Nanosuit.frag");

	// Model类
	Model mymodel("res/models/nanosuit/nanosuit.obj");

	// 点光源
	shader.Bind();
	shader.SetUniformVec3f("pointLights[0].position", glm::vec3(0.7f, 1.0f, 2.0f));
	shader.SetUniformVec3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	shader.SetUniformVec3f("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	shader.SetUniformVec3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.SetUniform1f("pointLights[0].constant", 1.0f);
	shader.SetUniform1f("pointLights[0].linear", 0.09f);
	shader.SetUniform1f("pointLights[0].quadratic", 0.032f);

	shader.SetUniformVec3f("pointLights[1].position", glm::vec3(2.3f, -3.3f, -4.0f));
	shader.SetUniformVec3f("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	shader.SetUniformVec3f("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	shader.SetUniformVec3f("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.SetUniform1f("pointLights[1].constant", 1.0f);
	shader.SetUniform1f("pointLights[1].linear", 0.09f);
	shader.SetUniform1f("pointLights[1].quadratic", 0.032f);

	// 渲染循环
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		shader.Bind();
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)WIDTH / (float)HEIGHT,
			0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.SetUniformMat4f("projection", 1, projection);
		shader.SetUniformMat4f("view", 1, view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		shader.SetUniformMat4f("model", 1, model);
		shader.SetUniformVec3f("viewPos", camera.GetPosition());
		mymodel.Draw(shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

// 窗口调整回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// 处理输入 - Escape键是否被按下
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && count == 0)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		count = 1;
	}
	else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && count == 1)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		count = 0;
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.001f;
		if (mixValue >= 1.0f) mixValue = 1.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.001f;
		if (mixValue <= 0.0f) mixValue = 0.0f;
	}
	// adjust accordingly
	float cameraSpeed = 2.5f * deltaTime; 
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}