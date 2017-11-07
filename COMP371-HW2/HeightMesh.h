#pragma once

#include "Mesh.h"

class HeightMesh: public Mesh {
public:
	HeightMesh();
	~HeightMesh();

	void setSkipsize(const GLuint& skipSize) { m_skipSize = skipSize; }
	void setStepsize(const GLfloat& stepSize) { m_stepSize = stepSize; }

private:
	unsigned char* m_HmData;
	GLint m_width, m_height, m_bpp;
	GLint m_skipSize;
	GLfloat m_stepSize;
};

