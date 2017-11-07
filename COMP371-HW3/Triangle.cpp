#pragma once

#include "Triangle.h"

Triangle::Triangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const ColorInfo& colorInfo) 
	: SceneObject(colorInfo)
	, m_v1(v1)
	, m_v2(v2)
	, m_v3(v3)
{}

Triangle::~Triangle() {}

/*
* Möller–Trumbore intersection algorithm
*/
bool Triangle::intersecting(const Ray& ray, float& t) const {
	const float EPSILON = 0.0000001;
	glm::vec3 edge1 = m_v2 - m_v1;
	glm::vec3 edge2 = m_v3 - m_v1;
	glm::vec3 h = glm::cross(ray.direction, edge2);
	float a = glm::dot(edge1, h);
	if (a > -EPSILON && a < EPSILON) { return false; }
	float f = 1 / a;
	glm::vec3 s = ray.origin - m_v1;
	float u = f * glm::dot(s, h);
	if (u < 0.0 || u > 1.0) { return false; }
	glm::vec3 q = glm::cross(s, edge1);
	float v = f * glm::dot(ray.direction, q);
	if (v < 0.0 || u + v > 1.0) { return false; }
	t = f * glm::dot(edge2, q);
	if (t > EPSILON) { return true; }
	else return false;
}

glm::vec3 Triangle::getNormal(const glm::vec3 point) const {
	glm::vec3 side1 = m_v2 - m_v1;
	glm::vec3 side2 = m_v3 - m_v1;
	return glm::normalize(glm::cross(side1, side2));
}
