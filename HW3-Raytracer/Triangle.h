#pragma once

#include "glm\glm.hpp"

#include "SceneObject.h"

class Triangle : public SceneObject {
public:
	Triangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const ColorInfo& colorInfo);
	virtual ~Triangle();

	glm::vec3 getV1() const { return m_v1; }
	glm::vec3 getV2() const { return m_v2; }
	glm::vec3 getV3() const { return m_v3; }

	virtual bool intersecting(const Ray& ray, float& t) const override;
	virtual glm::vec3 getNormal(const glm::vec3 point) const override;

private:
	glm::vec3 m_v1, m_v2, m_v3;
};