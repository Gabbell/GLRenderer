#include "GL/glew.h"	// include GL Extension Wrangler
#include "GLFW/glfw3.h"	// include GLFW helper library
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <math.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/random.hpp"

#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"

#define STB_IMAGE_IMPLEMENTATION
#define PRIMITIVE_RESTART 0xFFFFFFFF
#include "stb_image.h"

using namespace std;

enum class HMSTEP {
	ORIGINAL, SKIPSIZED, CR1, CR2
};

//Window dimensions
GLint screenWidth = 800, screenHeight = 800;

glm::mat4 projection_matrix;
glm::mat4 view_matrix;
glm::mat4 model_matrix;

//Camera
Camera camera(glm::vec3(344.5f,344.5f,689.0f));
GLboolean firstMouse = true;
GLdouble lastY;
GLdouble lastX;

//Shaders
Shader shader;

//Meshes
Mesh hmMesh;
Mesh hmMeshOriginal;
Mesh hmMeshSkipsized;
Mesh hmMeshCr1;
Mesh hmMeshCr2;

//Timing
GLuint frames = 0;
GLfloat counter = 0;
GLfloat delta = 0;
GLfloat currentTime = 0;

//Data for different steps
HMSTEP currentStep = HMSTEP::ORIGINAL;
std::vector<glm::vec3> hmVertsOriginal;
std::vector<glm::vec3> hmVertsSkipsized;
std::vector<glm::vec3> hmVertsCr1;
std::vector<glm::vec3> hmVertsCr2;
std::vector<GLuint> hmIndicesOriginal;
std::vector<GLuint> hmIndicesSkipsized;
std::vector<GLuint> hmIndicesCr1;
std::vector<GLuint> hmIndicesCr2;

//Other constants & variables
GLint currentRenderingMode = GL_TRIANGLE_STRIP;
GLint hmWidth, hmHeight, hmBpp;
unsigned char* hmData;
GLuint skipSize;
GLfloat stepSize;

//Prototypes
void render();
void hmParse();
void setupMeshes();
std::vector<GLuint> hmGenIndices(GLuint newWidth, GLuint newHeight);
std::vector<GLuint> hmGenIndices2(GLuint newWidth, GLuint newHeight);
std::queue<glm::vec3> linearInterpolation(glm::vec3 p0, glm::vec3 p1);
std::queue<glm::vec3> crInterpolation(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
void update(GLfloat delta);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void window_size_callback(GLFWwindow* window, int width, int height);

// The MAIN function, from here we start the application and run the game loop
int main() {

	std::cout << "Please enter a skip size: " << std::endl;
	std::cin >> skipSize;
	std::cout << "Please enter a step size: " << std::endl;
	std::cin >> stepSize;
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

	//Parsing and setting up meshes
	setupMeshes();

	//Start off with first original parsing mesh
	hmMesh = hmMeshOriginal;

	//OpenGL state configurations
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(PRIMITIVE_RESTART);

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

//Fill all the vertex vectors according to the 4 steps
void hmParse() {
	GLuint currentPixel = 0;
	GLuint newWidth, newHeight;

	//ORIGINAL PARSING
	for (GLfloat i = 0; i < hmWidth; i++) {
		for (GLfloat j = 0; j < hmHeight; j++) {
			hmVertsOriginal.push_back(glm::vec3(i, j, (GLfloat)hmData[currentPixel]));
			currentPixel++;
		}
	}
	hmIndicesOriginal = hmGenIndices(hmWidth, hmHeight);

	//SKIPSIZED PARSING
	currentPixel = 0;
	for (GLfloat i = 0; i < hmWidth; i += skipSize) {
		for (GLfloat j = 0; j < hmHeight; j += skipSize) {
			hmVertsSkipsized.push_back(glm::vec3(i, j, (GLfloat)hmData[currentPixel]));
			currentPixel += skipSize;
		}
		currentPixel = i * hmWidth; //Make sure the skipSize does not disturb the parsing by not being a divisor of the width
	}
	//Updating dimensions
	newWidth = ceil((GLfloat)hmWidth / skipSize);
	newHeight = ceil((GLfloat)hmHeight / skipSize);
	//Generating new indices
	hmIndicesSkipsized = hmGenIndices(newWidth, newHeight);
	
	//CR1 PARSING, using previously generated data from skipsized parsing
	for (GLuint i = 0; i < hmVertsSkipsized.size(); i++) {
		//Pushing original point from skipsized parsing
		hmVertsCr1.push_back(hmVertsSkipsized.at(i));

		GLint magicNumber = i - newWidth + 2; //It's the magic number. Don't ask questions. I made a drawing, trust me.
		if ((i + 1) % newWidth == 0) {
			//No need to calculate this point. End point of the row.
		}
		else if (i % newWidth == 0 || magicNumber % newWidth == 0) {
			//Special case first and last point
			glm::vec3 p0(hmVertsSkipsized.at(i));
			glm::vec3 p1(hmVertsSkipsized.at(i + 1));

			//Getting new points and pushing them
			std::queue<glm::vec3> newPoints = linearInterpolation(p0, p1);
			GLuint queueSize = newPoints.size();
			for (GLuint j = 0; j < queueSize; j++) {
				hmVertsCr1.push_back(newPoints.front());
				newPoints.pop();
			}
		}
		else {
			//Perform Catmull-Rom
			glm::vec3 p0(hmVertsSkipsized.at(i - 1));
			glm::vec3 p1(hmVertsSkipsized.at(i));
			glm::vec3 p2(hmVertsSkipsized.at(i + 1));
			glm::vec3 p3(hmVertsSkipsized.at(i + 2));

			std::queue<glm::vec3> newPoints(crInterpolation(p0, p1, p2, p3));
			GLuint queueSize = newPoints.size();
			for (GLuint j = 0; j < queueSize; j++) {
				hmVertsCr1.push_back(newPoints.front());
				newPoints.pop();
			}
		}
	}
	//Updating width
	newWidth += (newWidth - 1)*((1 / stepSize) - 1);
	hmIndicesCr1 = hmGenIndices(newWidth, newHeight);
	
	//CR2 PARSING, using previously generated data from CR1
	GLint magicNumber = newWidth*(newHeight - 2); //Yeah I should have thought this through
	GLuint queueSize = (1 / stepSize) - 1;

	for (GLuint y = 0; y < newWidth; y++) {
		for(GLuint x = 0; x < newHeight; x++){
			hmVertsCr2.push_back(hmVertsCr1.at(x * newWidth + y));

			if (x == newHeight-1) {
				//No need to calculate this point.
			}
			else if (x == 0 || x == newHeight-2) {
				//Special case first and last point
				glm::vec3 p0(hmVertsCr1.at(x * newWidth + y));
				glm::vec3 p1(hmVertsCr1.at((x + 1) * newWidth + y));

				//Getting new points and pushing them
				std::queue<glm::vec3> newPoints(linearInterpolation(p0, p1));
				for (GLuint j = 0; j < queueSize; j++) {
					hmVertsCr2.push_back(newPoints.front());
					newPoints.pop();
				}
			}
			else {
				//Perform Catmull-Rom
				glm::vec3 p0(hmVertsCr1.at((x - 1) * newWidth + y));
				glm::vec3 p1(hmVertsCr1.at(x * newWidth + y));
				glm::vec3 p2(hmVertsCr1.at((x + 1) * newWidth + y));
				glm::vec3 p3(hmVertsCr1.at((x + 2) * newWidth + y));

				std::queue<glm::vec3> newPoints(crInterpolation(p0, p1, p2, p3));
				for (GLuint j = 0; j < queueSize; j++) {
					hmVertsCr2.push_back(newPoints.front());
					newPoints.pop();
				}
			}
		}
	}
	newHeight += (newHeight - 1)*((1 / stepSize) - 1);
	hmIndicesCr2 = hmGenIndices2(newWidth, newHeight);
}

std::queue<glm::vec3> crInterpolation(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	std::queue<glm::vec3> newPoints;
	for (GLfloat u = stepSize; u < 1; u += stepSize) {
		glm::vec3 newPoint(0.5f*((2.0f * p1) + (-p0 + p2)*u + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3)*u*u + (-p0 + 3.0f * p1 - 3.0f * p2 + p3)*u*u*u));
		newPoints.push(newPoint);
	}
	return newPoints;
}

std::queue<glm::vec3> linearInterpolation(glm::vec3 p0, glm::vec3 p1) {
	std::queue<glm::vec3> newPoints;
	for (GLfloat u = stepSize; u < 1; u += stepSize) {
		glm::vec3 newPoint(p0 + u *(p1 - p0));
		newPoints.push(newPoint);
	}
	return newPoints;
}

//Generate indices using TRIANGLE_STRIP
std::vector<GLuint> hmGenIndices(GLuint newWidth, GLuint newHeight) {
	std::vector<GLuint> hmIndices;
	for (GLint x = 0; x < (newWidth - 1); x++) {
		for (GLint y = 0; y < newHeight; y++) {
			hmIndices.push_back((y*newWidth) + x + 1);
			hmIndices.push_back((y*newWidth) + x);
		}
		hmIndices.push_back(PRIMITIVE_RESTART);
	}
	return hmIndices;
}
std::vector<GLuint> hmGenIndices2(GLuint newWidth, GLuint newHeight) {
	std::vector<GLuint> hmIndices;
	for (GLint x = 0; x < (newHeight - 1); x++) {
		for (GLint y = 0; y < newWidth; y++) {
			hmIndices.push_back((y*newHeight) + x + 1);
			hmIndices.push_back((y*newHeight) + x);
		}
		hmIndices.push_back(PRIMITIVE_RESTART);
	}
	return hmIndices;
}

void setupMeshes() {
	std::cout << "-------------- LOADING MESHES --------------" << std::endl;
	//Clearing data from previous runs if any
	hmVertsOriginal.clear();
	hmVertsSkipsized.clear();
	hmVertsCr1.clear();
	hmVertsCr2.clear();

	hmIndicesOriginal.clear();
	hmIndicesSkipsized.clear();
	hmIndicesCr1.clear();
	hmIndicesCr2.clear();

	//Parsing and setting meshes
	hmParse();
	hmMeshOriginal = Mesh(hmVertsOriginal, hmIndicesOriginal);
	hmMeshSkipsized = Mesh(hmVertsSkipsized, hmIndicesSkipsized);
	hmMeshCr1 = Mesh(hmVertsCr1, hmIndicesCr1);
	hmMeshCr2 = Mesh(hmVertsCr2, hmIndicesCr2);

	std::cout << "-------------- LOADING COMPLETE --------------" << std::endl;
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
	glDrawElements(currentRenderingMode, hmMesh.getIndices().size(), GL_UNSIGNED_INT, nullptr);
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
		currentRenderingMode = GL_TRIANGLE_STRIP;

	//Parsing steps
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && currentStep != HMSTEP::ORIGINAL) {
		currentStep = HMSTEP::ORIGINAL;
		hmMesh = hmMeshOriginal;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && currentStep != HMSTEP::SKIPSIZED) {
		currentStep = HMSTEP::SKIPSIZED;
		hmMesh = hmMeshSkipsized;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && currentStep != HMSTEP::CR1) {
		currentStep = HMSTEP::CR1;
		hmMesh = hmMeshCr1;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && currentStep != HMSTEP::CR2) {
		currentStep = HMSTEP::CR2;
		hmMesh = hmMeshCr2;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		Camera camera(glm::vec3(344.5f, 344.5f, 689.0f));
		std::cout << "What skip size do you want? " << std::endl;
		std::cin >> skipSize;
		setupMeshes();
	}
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