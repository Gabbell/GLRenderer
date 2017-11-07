#include "Camera.h"

#include <iostream>

// Default camera values
const float YAW = 0.0f;
const float PITCH = 0.0f;

Camera::Camera(glm::vec3 position, float fov, float focalLength, float aspectRatio) :
	m_position(position),
	m_fov(fov),
	m_yaw(YAW),
	m_pitch(PITCH),
	m_focalLength(focalLength),
	m_aspectRatio(aspectRatio)
{
}

Camera::~Camera() {
}
