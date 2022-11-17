#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Geometry.h>

enum class CameraMovement
{
	FORWARD = 0,
	BACKWARD = 1,
	LEFT = 2,
	RIGHT = 3
};

static const float YAW = -90.0f;	// -90.0f
static const float PITCH = 0.0f;
static const float SPEED = 2.5f;
static const float SENSITIVITY = 0.01f;
static const float ZOOM = 45.0f;

class Camera
{
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW, float pitch = PITCH);
	
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	~Camera();

	glm::mat4 GetViewMatrix() const;
	void ProcessKeyboard(CameraMovement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yoffset);

	void UpdateProjection(float fov, float near, float far, float aspectRatio);
	void SetTranslationDelta(glm::vec3 direction, float amount);
	void SetRotationDelta(glm::vec3 angles);
	void SetPosition(glm::vec3 position);

	float GetZoom() const { return Zoom; }
	glm::vec3 GetPosition() const { return Position; }
	glm::vec3 GetFront() const { return Front; }
	glm::vec3 GetUp() const { return Up; }
	glm::vec3 GetRight() const { return Right; }

	const glm::quat& GetOrientation() const { return Orientation; }
	const glm::mat4& GetProjection() const { return Projection; }
	const glm::mat4& GetViewProjection() const { return ViewProjection; }

public:
	void UpdateCameraVectors();

public:
	mutable bool m_CameraMoved{ false };

private:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	glm::mat4 Projection;
	glm::mat4 ViewProjection;
	glm::mat4 PrevViewProjection;
	glm::mat4 Rotate;
	glm::mat4 View;
	glm::mat4 Translate;

	glm::quat Orientation;

	float Yaw;
	float Pitch;
	float Roll;

	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	float m_Near, m_Far;
	float m_AspectRatio;

	Renderer::Plane m_NearPlane;
	Renderer::Plane m_FarPlane;
	Renderer::Plane m_LeftPlane;
	Renderer::Plane m_RightPlane;
	Renderer::Plane m_TopPlane;
	Renderer::Plane m_BottomPlane;

	Renderer::Frustum m_Frustum;
};