#pragma once

#include "Sphere.h"

#include <algorithm>

Sphere::Sphere(const glm::vec3& position, float radius, const ColorInfo& colorInfo)
	: SceneObject(colorInfo)
	, m_position(position)
	, m_radius(radius)
{}

Sphere::~Sphere() {}

bool Sphere::intersecting(const Ray& ray, float& t) const {
	float b = 2.0f * (ray.direction.x*(ray.origin.x - m_position.x) + ray.direction.y*(ray.origin.y - m_position.y) + ray.direction.z*(ray.origin.z - m_position.z));
	float c = pow((ray.origin.x - m_position.x), 2) + pow((ray.origin.y - m_position.y), 2) + pow((ray.origin.z - m_position.z), 2) - pow(m_radius, 2);
	float discriminant = pow(b, 2) - (4 * c);
	if (discriminant > 0) {
		float t0 = (-b + sqrt(discriminant)) / 2.0f;
		float t1 = (-b - sqrt(discriminant)) / 2.0f;
		if (t0 > 0 && t1 > 0) {
			t = std::min(t0, t1);
			return true;
		}
	}
	return false;
}

glm::vec3 Sphere::getNormal(const glm::vec3 point) const {
	return glm::normalize(point - m_position);
}

