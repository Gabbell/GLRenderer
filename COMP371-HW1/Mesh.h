#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "GL/glew.h"
class Mesh
{
public:
	Mesh(const char* objectName);
	Mesh(std::vector<glm::vec3> vertices);
	Mesh();
	~Mesh();

	std::vector<glm::vec3> getVerts();
	std::vector<glm::vec3> getNorms();
	std::vector<glm::vec2> getUvs();
	std::vector<glm::vec3> getPositions();
	GLuint getVBO();
	GLuint getNormsVBO();
	GLuint getVAO();

	void setVerts(std::vector<glm::vec3> vertices);
	void setNorms(std::vector<glm::vec3> normals);
	void setUvs(std::vector<glm::vec2> uvs);
	void setPositions(std::vector<glm::vec3> positions);
	void setPosition(glm::vec3 position);
	void setPosition(GLuint index, glm::vec3 position);

	void addPosition(glm::vec3 position);
	void removePosition(GLuint index);
	void translate(glm::vec3 translation);
	void clearPositions();

private:
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec2> m_uvs;
	std::vector<glm::vec3> m_positions;

	//For now
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_normsVBO;
};

