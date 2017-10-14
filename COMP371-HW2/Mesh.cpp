#include "Mesh.h"
#include "objloader.hpp" 

Mesh::Mesh(const char* objectName) {
	loadOBJ(objectName, m_vertices, m_normals, m_uvs);

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_normsVBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), m_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_normsVBO);
	glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(glm::vec3), m_normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Mesh::Mesh(std::vector<glm::vec3> vertices) {
	m_vertices = vertices;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_normsVBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), m_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<GLuint> indices) {
	m_vertices = vertices;
	m_indices = indices;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_normsVBO);
	glGenBuffers(1, &m_EBO);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), m_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Mesh::Mesh() {

}

//Getters
std::vector<glm::vec3> Mesh::getVerts() {
	return m_vertices;
}
std::vector<GLuint> Mesh::getIndices() {
	return m_indices;
}
std::vector<glm::vec3> Mesh::getNorms() {
	return m_normals;
}
std::vector<glm::vec2> Mesh::getUvs() {
	return m_uvs;
}
GLuint Mesh::getVAO() {
	return m_VAO;
}
GLuint Mesh::getEBO() {
	return m_EBO;
}
GLuint Mesh::getVBO() {
	return m_VBO;
}
GLuint Mesh::getNormsVBO() {
	return m_normsVBO;
}
std::vector<glm::vec3> Mesh::getPositions() {
	return m_positions;
}

//Setters
void Mesh::setVerts(std::vector<glm::vec3> vertices) {
	m_vertices = vertices;
}
void Mesh::setIndices(std::vector<GLuint> indices) {
	m_indices = indices;
}
void Mesh::setNorms(std::vector<glm::vec3> normals) {
	m_normals = normals;
}
void Mesh::setUvs(std::vector<glm::vec2> uvs) {
	m_uvs = uvs;
}
void Mesh::setPositions(std::vector<glm::vec3> positions) {
	m_positions = positions;
}
void Mesh::setPosition(glm::vec3 position) {
	m_positions[0] = position;
}
void Mesh::setPosition(GLuint index, glm::vec3 position) {
	m_positions[index] = position;
}

//Other functions
void Mesh::addPosition(glm::vec3 position) {
	m_positions.push_back(position);
}
void Mesh::removePosition(GLuint index) {
	m_positions.erase(m_positions.begin() + index);
}
void Mesh::translate(glm::vec3 translation) {
	m_positions[0] += translation;
}
void Mesh::clearPositions() {
	m_positions.clear();
}

Mesh::~Mesh() {
}
