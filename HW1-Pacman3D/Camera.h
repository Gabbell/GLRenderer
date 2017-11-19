#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

class Camera {
public:
	Camera(glm::vec3 position, glm::vec3 front, GLfloat yaw, GLfloat pitch);
	glm::mat4 getViewMatrix();
	glm::mat4 getOrientation();
	void updateCameraVectors();
	void translate(glm::vec3 translationVec);
	void zoom(GLfloat fov);
	void swivelPitch(GLfloat angle);
	void swivelYaw(GLfloat angle);
	void arcballPitch(GLfloat angle);
	void arcballYaw(GLfloat angle);

	~Camera();

	//World Parameters
	glm::vec3 m_center = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	//Camera Vectors
	glm::vec3 m_position;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_front = glm::vec3(0.0f, 0.0f, -1.0f);

	//Camera Angles & FOV
	GLfloat m_yaw;
	GLfloat m_pitch;
	GLfloat m_zoom;

	//Camera Properties
	GLfloat mouseSensitivity = 0.1f;
	GLfloat velocity = 1.0f;

	//Camera States
	GLboolean zoomMode = false;
	GLboolean panMode = false;
	GLboolean tiltMode = false;
};

