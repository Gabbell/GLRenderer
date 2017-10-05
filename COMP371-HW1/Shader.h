#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include <string>

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	void use();
	void setFloat(const std::string &name, GLfloat value) const;
	void setVec4(const std::string &name, const glm::vec4 &value) const;
	void setMat3(const std::string &name, const glm::mat3 &mat) const;
	void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
	GLuint m_id;
};

