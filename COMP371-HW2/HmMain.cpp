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

//Colours
glm::vec4 redColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

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
GLint currentRenderingMode = GL_POINTS;
GLuint skipSize;
GLint hmWidth, hmHeight, hmBpp;
unsigned char* hmData;
std::vector<glm::vec3> hmVerts;

//Prototypes
void gameSetup();
void render();
void hmParse();
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	shader = Shader("../shaders/vertex.shader", "../shaders/fragment.shader");
	shader.use();

	//Object Loading
	hmData = stbi_load("../textures/heightmapdemo.bmp", &hmWidth, &hmHeight, &hmBpp, 1); //Colours are for weaklings

	hmParse();
	hmMesh = Mesh(hmVerts);

	//OpenGL state configurations
	glEnable(GL_DEPTH_TEST);

	// Game loop
	while (!glfwWindowShouldClose(window)) {
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		currentTime = glfwGetTime();

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
	for (GLfloat i = 0; i < hmWidth; i++) {
		for (GLfloat j = 0; j < hmHeight; j += skipSize) {
			hmVerts.push_back(glm::vec3(i, j, (GLfloat)hmData[currentPixel]));
			currentPixel += skipSize;
		}
	}
}

void update(GLfloat deltaSeconds) {

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
	shader.setVec4("drawColor", redColor);
	shader.setMat4("model_matrix", model_matrix);
	glDrawArrays(currentRenderingMode, 0, hmVerts.size());
	glBindVertexArray(0);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	//Exit when escape is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	std::cout << "callback called" << glm::linearRand(0,100) << std::endl;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(Camera_Movement::FORWARD);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
		camera.processKeyboard(Camera_Movement::BACKWARD);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
		camera.processKeyboard(Camera_Movement::LEFT);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
		camera.processKeyboard(Camera_Movement::RIGHT);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	//Get the current cursor position and store them in lastX and lastY
	glfwGetCursorPos(window, &lastX, &lastY);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	//Special case if it is first time mouse entered the screen
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

void window_size_callback(GLFWwindow* window, int width, int height) {
	screenWidth = width;
	screenHeight = height;
	glViewport(0, 0, width, height);
}