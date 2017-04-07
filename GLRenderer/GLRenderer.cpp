#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <iostream>
#include <exception>
#include <string>
#include <fstream>
#include <sstream>

std::string loadSrc(std::string);

int main()
{
	GLFWwindow* window;

	/* Initializing GLFW */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	std::cout << "Status: Using GLFW " << glfwGetVersionString() << std::endl;

	/* Initializing GLEW */
	if (glewInit() != GLEW_OK) {
		std::cout << "Error initializing GLEW" << std::endl;
		return 1;
	}
	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

	std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;

	/* Rendering Triangle */

	GLuint vaoID = 0;
	glGenVertexArrays(1, &vaoID);

	GLuint vboID = 0;
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	GLfloat vertex[] = {
		-0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
	glBindVertexArray(vaoID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	GLint success;
	GLchar infoLog[512];
	/* Vertex Shader Stuff */

	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	
	std::string srcString = loadSrc("../shaders/passthrough.vert");
	const GLchar* vertexShaderSource = srcString.c_str();
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Error: Vertex Shader failed to compile\n" << infoLog << std::endl;
		return 1;
	}

	/* Fragment Shader Stuff */

	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	srcString = loadSrc("../shaders/passthrough.frag");
	const GLchar* fragmentShaderSource = srcString.c_str();
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "Error: Fragment Shader failed to compile\n" << infoLog << std::endl;
		return 1;
	}

	/* Shader Program*/

	GLuint shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Error: Shader Program failed to link\n" << infoLog << std::endl;
		return 1;
	}

	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Clear */
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vaoID);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteBuffers(1, &vboID);
	glDeleteVertexArrays(1, &vaoID);
	glfwTerminate();
	return 0;
}

std::string loadSrc(std::string filePath) {
	std::ifstream input(filePath);

	if (!input) {
		input.close();
		throw std::runtime_error("Failed to load source file: " + filePath);
	}

	std::stringstream sstream;

	while (!input.eof()) {
		char character = input.get();

		if (input.eof()) {
			break;
		}
		sstream << character;
	}

	input.close();

	return sstream.str();
}