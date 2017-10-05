#include "Camera.h"

#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 front, GLfloat yaw, GLfloat pitch) {
	m_position = position;
	m_up = m_worldUp;
	m_yaw = yaw;
	m_pitch = pitch;
	m_zoom = 45.0f; //Default zoom value

	updateCameraVectors();
}

//Change camera position
void Camera::translate(glm::vec3 translationVec) {
	m_position += translationVec;
}
void Camera::zoom(GLfloat fov) {
	m_zoom -= fov;
}
void Camera::swivelPitch(GLfloat angle) {
	m_pitch += angle;
	updateCameraVectors();
}
void Camera::swivelYaw(GLfloat angle) {
	m_yaw -= angle;
	updateCameraVectors();
}
void Camera::arcballPitch(GLfloat angle) {
	//Getting vector from origin to camera position
	glm::vec3 targetToCamera = m_position - m_center;

	//Rotating it on the x axis by a certain angle
	glm::mat4 arcRotation(1.0f);
	arcRotation = glm::rotate(arcRotation, angle, glm::vec3(1.0f,0.0f,0.0f));
	glm::vec3 targetToNewPosition = arcRotation * glm::vec4(targetToCamera, 1.0f);

	//Translating camera position
	m_position += (targetToNewPosition - m_position);

	//Adjusting pitch
	m_pitch -= glm::degrees(angle);

	updateCameraVectors();
}
void Camera::arcballYaw(GLfloat angle) {
	//Getting vector from origin to camera position
	glm::vec3 targetToCamera = m_position - m_center;

	//Rotating it on the z axis by a certain angle
	glm::mat4 arcRotation(1.0f);
	arcRotation = glm::rotate(arcRotation, angle, glm::vec3(0.0f,0.0f,1.0f));
	glm::vec3 targetToNewPosition = arcRotation * glm::vec4(targetToCamera, 1.0f);

	//Translating camera position
	m_position += (targetToNewPosition - m_position);

	//Adjusting yaw
	m_yaw -= glm::degrees(angle);

	updateCameraVectors();
}

//Update all camera vectors with new data
void Camera::updateCameraVectors() {
	m_front = glm::vec3(glm::inverse(getOrientation()) * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
	m_right = glm::vec3(glm::inverse(getOrientation()) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	m_up = glm::vec3(glm::inverse(getOrientation()) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	m_front = glm::normalize(m_front);
	m_right = glm::normalize(m_right);
	m_up = glm::normalize(m_up);
}

//Returns equivalent rotation matrix of the camera
glm::mat4 Camera::getOrientation() {
	glm::mat4 rotation(1.0f);
	rotation = glm::rotate(rotation, glm::radians(m_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(m_yaw), glm::vec3(0.0f, 0.0f, 1.0f));
	return rotation;
}

//Returns lookAt matrix
glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(m_position, m_position + m_front, m_up);
}

Camera::~Camera(){
}
