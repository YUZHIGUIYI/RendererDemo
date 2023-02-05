#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

Camera::Camera(glm::vec3 position /*= glm::vec3(0.0f, 0.0f, 0.0f)*/, glm::vec3 up /*= glm::vec3(0.0f, 1.0f, 0.0f)*/, float yaw /*= YAW*/, float pitch /*= PITCH*/)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY),
		Zoom(ZOOM)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	Roll = 0.0f;

	m_Near = 0.1f;
	m_Far = 150.0f;
	m_AspectRatio = 4.0 / 3.0;

	Orientation = glm::quat();
	Projection = glm::perspective(glm::radians(ZOOM), m_AspectRatio, m_Near, m_Far);
	PrevViewProjection = glm::mat4(1.0f);

	UpdateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY),
		Zoom(ZOOM)
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	Roll = 0.0f;

	m_Near = 0.1f;
	m_Far = 150.0f;
	m_AspectRatio = 4.0 / 3.0;

	Orientation = glm::quat();
	Projection = glm::perspective(glm::radians(ZOOM), m_AspectRatio, m_Near, m_Far);
	PrevViewProjection = glm::mat4(1.0f);

	UpdateCameraVectors();
}

Camera::~Camera()
{

}

glm::mat4 Camera::GetViewMatrix() const
{
	//return glm::lookAt(Position, Position + Front, Up);	// has been discarded
	return View;
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
	// has been discarded
	float velocity = MovementSpeed * deltaTime;
	switch (direction)
	{
	case CameraMovement::FORWARD:
		Position += Front * velocity;
		break;
	case CameraMovement::BACKWARD:
		Position -= Front * velocity;
		break;
	case CameraMovement::LEFT:
		Position -= Right * velocity;
		break;
	case CameraMovement::RIGHT:
		Position += Right * velocity;
		break;
	default:
		break;
	}
	// prohibit fly
	//Position.y = 0.0f;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch /*= true*/)
{
	// has been discarded
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	if (constrainPitch)
	{
		if (Pitch > 89.0f) Pitch = 89.0f;
		if (Pitch < -89.0f) Pitch = -89.0f;
	}

	UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	Zoom -= yoffset;
	if (Zoom < 1.0f) Zoom = 1.0f;
	if (Zoom > 50.0f) Zoom = 50.0f;

	UpdateProjection(Zoom, m_Near, m_Far, m_AspectRatio);
}

void Camera::UpdateProjection(float fov, float near, float far, float aspectRatio)
{
	m_Near = near;
	m_Far = far;
	m_AspectRatio = aspectRatio;
	Projection = glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

void Camera::SetTranslationDelta(glm::vec3 direction, float amount)
{
	Position += direction * amount;
}

void Camera::SetRotationDelta(glm::vec3 angles)
{
	Yaw = glm::radians(angles.y);
	Pitch = glm::radians(angles.x);
	Roll = glm::radians(angles.z);
}

void Camera::SetPosition(glm::vec3 position)
{
	Position = position;
}

void Camera::UpdateCameraVectors()
{
	// old method
	//glm::vec3 front;
	//front.x = std::cos(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));
	//front.y = std::sin(glm::radians(Pitch));
	//front.z = std::sin(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));
	//Front = glm::normalize(front);
	//Right = glm::normalize(glm::cross(Front, WorldUp));
	//Up = glm::normalize(glm::cross(Right, Front));

	// new method via quaternion
	glm::quat qPitch = glm::angleAxis(Pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat qYaw = glm::angleAxis(Yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat qRoll = glm::angleAxis(Roll, glm::vec3(0.0f, 0.0f, 1.0f));

	Orientation = qPitch * Orientation;
	Orientation = Orientation * qYaw;
	Orientation = qRoll * Orientation;
	Orientation = glm::normalize(Orientation);

	Rotate = glm::mat4_cast(Orientation);
	Front = glm::conjugate(Orientation) * glm::vec3(0.0f, 0.0f, -1.0f);

	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));

	Translate = glm::translate(glm::mat4(1.0f), -Position);
	View = Rotate * Translate;
	PrevViewProjection = ViewProjection;
	ViewProjection = Projection * View;

	Renderer::FrustumFromMatrix(m_Frustum, ViewProjection);
}

