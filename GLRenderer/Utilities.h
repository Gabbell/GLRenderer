#pragma once
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <iostream>

static GLFWwindow* initWindow(GLuint width, GLuint height) {
	/* Initializing GLFW */
	if (!glfwInit())
		return nullptr;

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(width, height, "GLRenderer", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return nullptr;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	std::cout << "Status: Using GLFW " << glfwGetVersionString() << std::endl;

	/* Initializing GLEW */
	if (glewInit() != GLEW_OK) {
		std::cout << "Error initializing GLEW" << std::endl;
	}
	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

	std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;

	return window;
}

static GLuint genTexture(const char* texPath, GLuint unit) {
	unsigned int texture;
	glGenTextures(1, &texture);

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, texture);

	//Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Load and generate the texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(texPath, &width, &height, &nrChannels, 0);
	if (data && nrChannels == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else if (data && nrChannels == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture: " << texPath << std::endl;
	}

	stbi_image_free(data);

	return texture;
}
