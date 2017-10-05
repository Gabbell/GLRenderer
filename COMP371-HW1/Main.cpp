#include "stdafx.h"

#include "GL/glew.h"	// include GL Extension Wrangler
#include "GLFW/glfw3.h"	// include GLFW helper library
#include <iostream>
#include <string>
#include <fstream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/random.hpp"
#include "objloader.hpp"  //include the object loader

#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "Transform.h"

using namespace std;

enum Direction {
	UP, DOWN, LEFT, RIGHT
};

//Window dimensions
GLint screenWidth = 800, screenHeight = 800;

glm::vec3 input_scale = glm::vec3(1.0f);
glm::mat4 projection_matrix;
glm::mat4 view_matrix;
glm::mat4 model_matrix;

//Camera
Camera camera(glm::vec3(0.0f,0.0f,15.0f), glm::vec3(0.0f,0.0f,-1.0f), 0.0f, 0.0f);
GLboolean firstMouse = true;
GLdouble lastY;
GLdouble lastX;

//Meshes
Mesh meshPacman;
Mesh meshGrid;
Mesh meshAxes;
Mesh meshSpheres;
Mesh meshGhosts;

//Transforms
Transform transPacman;
Transform transGrid;
Transform transAxes;
Transform transSpheres;
Transform transGhosts;

//Colors
glm::vec4 yellowColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
glm::vec4 whiteColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 redColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
glm::vec4 greenColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
glm::vec4 blueColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

//Other constants & variables
Direction currentDirection = RIGHT;
Direction nextDirection = RIGHT;
GLfloat currentAngle = 0.0f;
GLint gridSize;
GLint gridBounds;
GLint currentRenderingMode = GL_TRIANGLES;
GLfloat enemyTimer;

//Prototypes
GLfloat determineAngle();
void generateGridVerts(std::vector<glm::vec3> &gridVerts);
void performGhostTranslation();
void checkGameOver();
void gameSetup();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void window_size_callback(GLFWwindow* window, int width, int height);

// The MAIN function, from here we start the application and run the game loop
int main() {
	std::cout << "Please enter a grid dimension (only odd numbers work): ";
	std::cin >> gridSize;
	gridBounds = gridSize / 2;

	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	std::cout << "Status: Using GLFW " << glfwGetVersionString() << std::endl;

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Load one cube", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;

	// Define the viewport dimensions
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);

	Shader shader = Shader("../shaders/vertex.shader", "../shaders/fragment.shader");
	shader.use();

	//Object Loading
	meshPacman = Mesh("../models/pacman.obj");
	meshPacman.addPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	meshSpheres = Mesh("../models/sphere.obj");
	meshGhosts = Mesh("../models/mazeghost.obj");

	//Generating axes vertices
	std::vector<glm::vec3> axesVerts;
	//X axis
	axesVerts.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	axesVerts.push_back(glm::vec3(4.0f, 0.0f, 0.0f));
	//Y axis
	axesVerts.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	axesVerts.push_back(glm::vec3(0.0f, 4.0f, 0.0f));
	//Z axis
	axesVerts.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	axesVerts.push_back(glm::vec3(0.0f, 0.0f, 4.0f));

	meshAxes = Mesh(axesVerts);

	//Generating grid vertices
	std::vector<glm::vec3> gridVerts;
	generateGridVerts(gridVerts);
	meshGrid = Mesh(gridVerts);

	srand(time(NULL));
	gameSetup();

	//OpenGL state configurations
	glEnable(GL_DEPTH_TEST);

	// Game loop
	while (!glfwWindowShouldClose(window)) {
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		//Clearing color & bits
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Creating matrices
		view_matrix = camera.getViewMatrix();
		projection_matrix = glm::perspective(glm::radians(camera.m_zoom), (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);

		//Setting the matrix uniforms
		shader.setMat4("view_matrix", view_matrix);
		shader.setMat4("projection_matrix", projection_matrix);
		
		//Grid rendering
		glBindVertexArray(meshGrid.getVAO());
		model_matrix = glm::mat4();
		model_matrix = glm::scale(model_matrix, glm::vec3(1.0f));
		shader.setVec4("drawColor", whiteColor);
		shader.setMat4("model_matrix", model_matrix);
		glDrawArrays(GL_LINES, 0, gridVerts.size());
		glBindVertexArray(0);

		//Axes rendering
		glBindVertexArray(meshAxes.getVAO());
		model_matrix = glm::mat4();
		model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, -1.0f));
		shader.setMat4("model_matrix", model_matrix);

		shader.setVec4("drawColor", redColor);
		glDrawArrays(GL_LINES, 0, 2);

		shader.setVec4("drawColor", greenColor);
		glDrawArrays(GL_LINES, 2, 2);

		shader.setVec4("drawColor", blueColor);
		glDrawArrays(GL_LINES, 4, 2);

		glBindVertexArray(0);
		
		//Sphere rendering
		glBindVertexArray(meshSpheres.getVAO());
		shader.setVec4("drawColor", redColor);
		for (unsigned int i = 0; i < meshSpheres.getPositions().size(); i++) {
			model_matrix = glm::mat4();
			model_matrix = glm::translate(model_matrix, meshSpheres.getPositions()[i]);
			model_matrix = glm::scale(model_matrix, input_scale * glm::vec3(0.2f));
			shader.setMat4("model_matrix", model_matrix);
			glDrawArrays(currentRenderingMode, 0, meshSpheres.getVerts().size());
		}
		glBindVertexArray(0);

		//Ghosts rendering
		performGhostTranslation();
		glBindVertexArray(meshGhosts.getVAO());
		shader.setVec4("drawColor", blueColor);
		for (unsigned int i = 0; i < meshGhosts.getPositions().size(); i++) {
			model_matrix = glm::mat4();
			model_matrix = glm::translate(model_matrix, meshGhosts.getPositions()[i]);
			model_matrix = glm::scale(model_matrix, input_scale * glm::vec3(0.05f));
			model_matrix = glm::rotate(model_matrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			shader.setMat4("model_matrix", model_matrix);
			glDrawArrays(currentRenderingMode, 0, meshGhosts.getVerts().size());
		}
		glBindVertexArray(0);

		//Pacman rendering
		glBindVertexArray(meshPacman.getVAO());
		model_matrix = glm::mat4();
		model_matrix = glm::translate(model_matrix, meshPacman.getPositions()[0]);
		model_matrix = glm::scale(model_matrix, input_scale * glm::vec3(0.025f));

		model_matrix = glm::rotate(model_matrix, glm::radians(currentAngle += determineAngle()), glm::vec3(0.0f, 0.0f, 1.0f));
		currentDirection = nextDirection;

		shader.setMat4("model_matrix", model_matrix);
		shader.setVec4("drawColor", yellowColor);
		glDrawArrays(currentRenderingMode, 0, meshPacman.getVerts().size());
		glBindVertexArray(0);
		
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

//Determine rotation angle based on direction changes
GLfloat determineAngle() {
	switch (nextDirection) {
		case UP:
			if (currentDirection == DOWN)
				return 180.0f;
			if (currentDirection == RIGHT)
				return 90.0f;
			if (currentDirection == LEFT)
				return -90.0f;
			else
				return 0.0f;
		case DOWN:
			if (currentDirection == UP)
				return 180.0f;
			if (currentDirection == RIGHT)
				return -90.0f;
			if (currentDirection == LEFT)
				return 90.0f;
			else
				return 0.0f;
		case RIGHT:
			if (currentDirection == UP)
				return -90.0f;
			if (currentDirection == DOWN)
				return 90.0f;
			if (currentDirection == LEFT)
				return 180.0f;
			else
				return 0.0f;
		case LEFT:
			if (currentDirection == UP)
				return 90.0f;
			if (currentDirection == DOWN)
				return -90.0f;
			if (currentDirection == RIGHT)
				return 180.0f;
			else
				return 0.0f;
	}
	return 0.0f;
}

void checkSphereCollisions() {
	for (int i = 0; i < meshSpheres.getPositions().size(); i++) {
		if (meshSpheres.getPositions()[i] == meshPacman.getPositions()[0]) {
			meshSpheres.removePosition(i);
		}
	}
}
bool checkGhostCollisions(GLuint index, GLfloat x, GLfloat y) {
	std::vector<glm::vec3> ghostPositions = meshGhosts.getPositions();
	glm::vec3 currentGhostPosition = ghostPositions[index];
	glm::vec3 newGhostPosition = glm::vec3(currentGhostPosition.x + x, currentGhostPosition.y + y, currentGhostPosition.z);
	for (int i = 0; i < ghostPositions.size(); i++) {
		if (i != index && ghostPositions[i] == newGhostPosition) {
			return true;
		}
	}
	return false;
}
void checkGameOver() {
	glm::vec3 pacmanPosition = meshPacman.getPositions()[0];
	for (int i = 0; i < meshGhosts.getPositions().size(); i++) {
		//GAME OVER
		if (pacmanPosition == meshGhosts.getPositions()[i]) {
			gameSetup();
		}
	}
}

//Pathfinding
void performGhostTranslation() {
	if (enemyTimer > 200) {
		enemyTimer = 0;
		std::vector<glm::vec3> ghostPositions = meshGhosts.getPositions();
		glm::vec3 pacmanPosition = meshPacman.getPositions()[0];

		for (int i = 0; i < ghostPositions.size(); i++) {
			glm::vec3 currentGhostPosition = ghostPositions[i];
			glm::vec3 ghostToPacman = pacmanPosition - currentGhostPosition;

			//Move in x
			if (abs(ghostToPacman.x) > abs(ghostToPacman.y)) {
				if (pacmanPosition.x - currentGhostPosition.x < 0 && !checkGhostCollisions(i, -1.0f, 0.0f)) {
					meshGhosts.setPosition(i, glm::vec3(currentGhostPosition.x - 1.0f, currentGhostPosition.y, currentGhostPosition.z));
				}
				else if (!checkGhostCollisions(i, 1.0f, 0.0f)) {
					meshGhosts.setPosition(i, glm::vec3(currentGhostPosition.x + 1.0f, currentGhostPosition.y, currentGhostPosition.z));
				}
			}
			//Move in y
			else {
				if (pacmanPosition.y - currentGhostPosition.y < 0 && !checkGhostCollisions(i, 0.0f, -1.0f)) {
					meshGhosts.setPosition(i, glm::vec3(currentGhostPosition.x, currentGhostPosition.y - 1.0f, currentGhostPosition.z));
				}
				else if (!checkGhostCollisions(i, 0.0f, 1.0f)) {
					meshGhosts.setPosition(i, glm::vec3(currentGhostPosition.x, currentGhostPosition.y + 1.0f, currentGhostPosition.z));
				}
			}
		}
		checkGameOver();
	}
	else {
		enemyTimer++;
	}
}

void generateGridVerts(std::vector<glm::vec3>& gridVerts) {
	GLfloat currentX = -(gridSize / 2);
	GLfloat currentY = -(gridSize / 2);
	for (int i = 0; i < gridSize; i++) {
		gridVerts.push_back(glm::vec3(currentX, currentY, 0));
		gridVerts.push_back(glm::vec3(currentX, (-currentY), 0));
		currentX++;
	}
	currentX = -(gridSize / 2);
	currentY = -(gridSize / 2);
	for (int i = 0; i < gridSize; i++) {
		gridVerts.push_back(glm::vec3(currentX, currentY, 0));
		gridVerts.push_back(glm::vec3((-currentX), currentY, 0));
		currentY++;
	}
}

//Setting up initial positions
void gameSetup() {
	meshSpheres.clearPositions();
	meshGhosts.clearPositions();
	meshPacman.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	//Generating random positions for spheres
	GLuint numberOfSpheres = glm::linearRand(1, 20);
	for (int i = 0; i < numberOfSpheres; i++) {
		meshSpheres.addPosition(glm::vec3(glm::linearRand(-gridBounds, gridBounds), glm::linearRand(-gridBounds, gridBounds), 0));
	}
	//Generating random positions for ghosts
	GLuint numberOfGhosts = glm::linearRand(3, 5);
	for (int i = 0; i < numberOfGhosts; i++) {
		meshGhosts.addPosition(glm::vec3(glm::linearRand(-gridBounds, gridBounds), glm::linearRand(-gridBounds, gridBounds), 0));
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	//Exit when escape is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//Camera movement keys
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.arcballPitch(0.015f);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.arcballPitch(-0.015f);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.arcballYaw(0.015f);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.arcballYaw(-0.015f);

	//Changing scale
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		input_scale += 0.5f;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		input_scale -= 0.5f;

	//Rendering modes
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		currentRenderingMode = GL_POINTS;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		currentRenderingMode = GL_LINES;
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		currentRenderingMode = GL_TRIANGLES;

	//Resetting camera
	if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
		camera = Camera(glm::vec3(0.0f, 0.0f, 15.0f), glm::vec3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f);

	//Random Player Location
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		meshPacman.setPosition(glm::vec3((int)glm::linearRand(-gridBounds, gridBounds), (int)glm::linearRand(-gridBounds, gridBounds), 0));

	//Player movement keys
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && meshPacman.getPositions()[0].y < (GLint)gridSize/2) {
		meshPacman.translate(glm::vec3(0.0f, 1.0f, 0.0f));
		nextDirection = UP;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && meshPacman.getPositions()[0].y > -(GLint)gridSize/2) {
		meshPacman.translate(glm::vec3(0.0f, -1.0f, 0.0f));
		nextDirection = DOWN;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && meshPacman.getPositions()[0].x > -(GLint)gridSize / 2) {
		meshPacman.translate(glm::vec3(-1.0f, 0.0f, 0.0f));
		nextDirection = LEFT;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && meshPacman.getPositions()[0].x < (GLint)gridSize / 2) {
		meshPacman.translate(glm::vec3(1.0f, 0.0f, 0.0f));
		nextDirection = RIGHT;
	}

	checkSphereCollisions();
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	//Get the current cursor position and store them in lastX and lastY
	glfwGetCursorPos(window, &lastX, &lastY);

	//Enabling different modes depending on button pressed
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		camera.zoomMode = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		camera.zoomMode = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		camera.panMode = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		camera.panMode = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		camera.tiltMode = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
		camera.tiltMode = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	//Special case if it is first time mouse entered the screen
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	//Calculating offset from original cursor position
	GLfloat yoffset = lastY - ypos;
	GLfloat xoffset = lastX - xpos;
	lastY = ypos;
	lastX = xpos;

	//Camera actions depending on mode
	if (camera.zoomMode) {
		camera.zoom(yoffset/50);
	}
	if (camera.panMode) {
		camera.translate(glm::vec3(xoffset/50,0.0f,0.0f));
	}
	if (camera.tiltMode) {
		camera.swivelPitch(yoffset/50);
	}
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	screenWidth = width;
	screenHeight = height;
	glViewport(0, 0, width, height);
}