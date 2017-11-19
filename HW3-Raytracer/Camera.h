#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

class Camera {
public:
	Camera(glm::vec3 position, float fov, float focalLength, float aspectRatio);

	float getFov() const { return m_fov; }
	float getAR() const { return m_aspectRatio; }
	glm::vec3 getPosition() const { return m_position; }
	float getFocalLength() const { return m_focalLength; }

	~Camera();
private:
	//World Parameters
	glm::vec3 m_center = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	//Camera Vectors
	glm::vec3 m_position;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_front = glm::vec3(0.0f, 0.0f, -1.0f);

	//Camera Parameters
	float m_yaw;
	float m_pitch;
	float m_fov;
	float m_focalLength;
	float m_aspectRatio;
};

