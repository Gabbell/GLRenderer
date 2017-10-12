#include "GL/glew.h"	// include GL Extension Wrangler
#include "GLFW/glfw3.h"	// include GLFW helper library
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/random.hpp"

#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

//Window dimensions
GLint screenWidth = 800, screenHeight = 800;

glm::mat4 projection_matrix;
glm::mat4 view_matrix;
glm::mat4 model_matrix;

//Camera
Camera camera(glm::vec3(344.5f,344.5f,344.5f), glm::vec3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f);
GLboolean firstMouse = true;
GLdouble lastY;
GLdouble lastX;

//Shaders
Shader shader;

//Meshes
Mesh hmMesh;

//Timing
GLuint frames = 0;
GLfloat counter = 0;
GLfloat delta = 0;
GLfloat currentTime = 0;

//Other constants & variables
GLint currentRenderingMode = GL_TRIANGLES;
GLint hmWidth, hmHeight, hmBpp;
unsigned char* hmData;
std::vector<glm::vec3> hmVerts;
GLuint skipSize;
GLuint stepSize;
GLfloat s = 0.5; //Tension parameter
glm::mat4 basisMatrix( //Column major
	-s,		2*s,	-s,		0,
	2-s,	s-3,	0,		1,
	s-2,	3-2*s,	s,		0,
	s,		-s,		0,		0
);

//Prototypes
void gameSetup();
void render();
void hmParse();
void catmullRomPass(std::vector<glm::vec3> verts);
void update(GLfloat delta);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void window_size_callback(GLFWwindow* window, int width, int height);

// The MAIN function, from here we start the application and run the game loop
int main() {

	std::cout << "Please enter a skip size: " << std::endl;
	std::cin >> skipSize;
	// Init GLFW
	glfwInit();

	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	std::cout << "Status: Using GLFW " << glfwGetVersionString() << std::endl;

	//GLFW init
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Height Map Demo", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// GLEW init
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;

	// Define the viewport dimensions
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	shader = Shader("../shaders/hw2.vert", "../shaders/hw2.frag");
	shader.use();

	//Object Loading
	hmData = stbi_load("../textures/heightmapdemo.bmp", &hmWidth, &hmHeight, &hmBpp, 1); //Colours are for weaklings

	hmParse();
	hmMesh = Mesh(hmVerts);

	//OpenGL state configurations
	glEnable(GL_DEPTH_TEST);

	// Game loop
	while (!glfwWindowShouldClose(window)) {
		currentTime = glfwGetTime();

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		update((GLfloat)delta);

		render();

		// Swap the screen buffers
		glfwSwapBuffers(window);

		delta = glfwGetTime() - currentTime;

		counter += delta;
		if (counter >= 1) {
			counter = 0;
			std::cout << "FPS: " << frames << std::endl;
			frames = 0;
		}
		else {
			frames++;
		}
	}

	//Freeing image data
	stbi_image_free(hmData);

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

//Setting up initial variables
void hmParse() {
	GLuint currentPixel = 0;
	std::vector<glm::vec3> hmRawVerts;

	for (GLfloat i = 0; i < hmWidth; i += skipSize) {
		for (GLfloat j = 0; j < hmHeight; j += skipSize) {
			hmRawVerts.push_back(glm::vec3(i, j, (GLfloat)hmData[currentPixel]));
			currentPixel += skipSize;
		}
		currentPixel = i * hmWidth; //Make sure the skipSize does not disturb the parsing by not being a divisor of the width
	}

	GLuint newWidth = hmWidth / skipSize;
	GLuint newHeight = hmHeight / skipSize;

	std::cout << "newWidth: " << newWidth << " newHeight: " << newHeight << std::endl;
	//Ordering vertices properly for rendering
	for (int row = 0; row < newWidth - 1; row++) {
		for (int col = 0; col < newHeight - 1; col++) {
			//All four vertices of the rectangle made by two triangles
			glm::vec3 topRight(hmRawVerts.at((row*newWidth) + col + 1));
			glm::vec3 topLeft(hmRawVerts.at((row*newWidth) + col));
			glm::vec3 bottomLeft(hmRawVerts.at((row + 1)*newWidth + col));
			glm::vec3 bottomRight(hmRawVerts.at((row + 1)*newWidth + col + 1));

			//CCW Order
			//First triangle
			hmVerts.push_back(topRight);
			hmVerts.push_back(topLeft);
			hmVerts.push_back(bottomLeft);
			//Second Triangle
			hmVerts.push_back(topRight);
			hmVerts.push_back(bottomLeft);
			hmVerts.push_back(bottomRight);
		}
	}
}

void catmullRomPass(std::vector<glm::vec3> verts) {
	for (int i = 0; i < verts.size(); i++) {
		glm::vec3 p0(verts[i]);
		glm::vec3 p1(verts[i + 1]);
		glm::vec3 p2(verts[i + 2]);
		glm::vec3 p3(verts[i + 3]);

		glm::mat3x4 controlMatrix(
			p0.x, p1.x, p2.x, p3.x,
			p0.y, p1.y, p2.y, p3.y,
			p0.z, p1.z, p2.z, p3.z
		);

		for (GLfloat u = 0; u < 1; u += stepSize) {
			glm::vec4 currentU(u*u*u, u*u, u, 1);
			glm::vec3 newPoint(currentU * basisMatrix * controlMatrix);
		}
	}
}

void update(GLfloat deltaSeconds) {
	camera.processMovement(delta);
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Clearing color & bits
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Creating matrices
	view_matrix = camera.getViewMatrix();
	projection_matrix = glm::perspective(glm::radians(camera.m_zoom), (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 2000.0f);

	//Setting the matrix uniforms
	shader.setMat4("view_matrix", view_matrix);
	shader.setMat4("projection_matrix", projection_matrix);

	//Mesh rendering
	glBindVertexArray(hmMesh.getVAO());
	model_matrix = glm::mat4();
	model_matrix = glm::scale(model_matrix, glm::vec3(1.0f));
	shader.setMat4("model_matrix", model_matrix);
	glDrawArrays(currentRenderingMode, 0, hmVerts.size());
	glBindVertexArray(0);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	//Exit when escape is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.setGoingForward(true);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
		camera.setGoingBackward(true);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.setGoingLeft(true);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.setGoingRight(true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
		camera.setGoingForward(false);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
		camera.setGoingBackward(false);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
		camera.setGoingLeft(false);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
		camera.setGoingRight(false);

	//Rendering Modes
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		currentRenderingMode = GL_POINTS;
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		currentRenderingMode = GL_TRIANGLES;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	//Get the current cursor position and store them in lastX and lastY
	glfwGetCursorPos(window, &lastX, &lastY);
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		camera.setFreeMode(true);
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		camera.setFreeMode(false);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (camera.getFreeMode()) {//Special case if it is first time mouse entered the screen
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.processMouseMovement(xoffset, yoffset, true);
	}
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	screenWidth = width;
	screenHeight = height;
	glViewport(0, 0, width, height);
}