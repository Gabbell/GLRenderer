#pragma once

#include "glm\glm.hpp"

#include "SceneObject.h"

class Plane : public SceneObject {
public:
	Plane(const glm::vec3& position, const glm::vec3& normal, const ColorInfo& colorInfo);
	virtual ~Plane();

	glm::vec3 getPosition() const { return m_position; }

	virtual bool intersecting(const Ray& ray, float& t) const override;
	virtual glm::vec3 getNormal(const glm::vec3 point) const { return m_normal; }

private:
	glm::vec3 m_position;
	glm::vec3 m_normal;
};