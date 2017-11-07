#pragma once

#include "Plane.h"

#include <algorithm>

Plane::Plane(const glm::vec3& position, const glm::vec3& normal, const ColorInfo& colorInfo)
	: SceneObject(colorInfo)
	, m_position(position)
	, m_normal(normal)
{}

Plane::~Plane() {}

bool Plane::intersecting(const Ray& ray, float& t) const {
	float denominator = glm::dot(m_normal, ray.direction);
	if (denominator != 0.0f) {
		t = -(glm::dot(m_normal, ray.origin) + glm::length(m_position)) / glm::dot(m_normal, ray.direction);
		return true;
	}
	return false;
}

