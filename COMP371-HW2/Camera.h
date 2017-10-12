#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

enum class Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera {
public:
	Camera(glm::vec3 position, glm::vec3 front, GLfloat yaw, GLfloat pitch);
	glm::mat4 getViewMatrix();
	glm::mat4 getOrientation();
	void setFreeMode(GLboolean state);
	void setGoingForward(GLboolean state);
	void setGoingBackward(GLboolean state);
	void setGoingRight(GLboolean state);
	void setGoingLeft(GLboolean state);
	GLboolean getFreeMode();
	GLboolean isGoingForward();
	GLboolean isGoingBackward();
	GLboolean isGoingRight();
	GLboolean isGoingLeft();
	void updateCameraVectors();
	void processMovement(GLfloat delta);
	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);
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

private:
	//Camera Properties
	GLfloat mouseSensitivity = 0.1f;
	GLfloat movementSpeed = 50.0f;

	//Camera States
	GLboolean freeMode = false;
	GLboolean goingForward = false;
	GLboolean goingBackward = false;
	GLboolean goingRight = false;
	GLboolean goingLeft = false;
};

