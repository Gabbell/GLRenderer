#pragma once

#include "glm\glm.hpp"

class Light{
public:
	Light(const glm::vec3& position, const glm::vec3& color);
	virtual ~Light();

	glm::vec3 getPosition() const { return m_position; }
	glm::vec3 getColor() const { return m_color; }

private:
	glm::vec3 m_position;
	glm::vec3 m_color;
};
