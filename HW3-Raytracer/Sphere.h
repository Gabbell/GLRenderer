#pragma once

#include "glm\glm.hpp"

#include "SceneObject.h"

class Sphere: public SceneObject{
public:
	Sphere(const glm::vec3& position, float radius, const ColorInfo& colorInfo);
	virtual ~Sphere();

	glm::vec3 getPosition() const { return m_position; }
	float getRadius() const { return m_radius; }

	virtual bool intersecting(const Ray& ray, float& t) const override;
	virtual glm::vec3 getNormal(const glm::vec3 point) const override;

private:
	glm::vec3 m_position;
	float m_radius;
};