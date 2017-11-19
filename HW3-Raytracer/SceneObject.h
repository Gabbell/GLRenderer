#pragma once

#include "glm\glm.hpp"

#include "Ray.h"

struct ColorInfo {
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float shininessFactor;
};

class SceneObject
{
public:
	SceneObject(const ColorInfo& colorInfo);
	virtual ~SceneObject();

	glm::vec3 getAmbientColor() const { return m_colorInfo.ambientColor; }
	glm::vec3 getDiffuseColor() const { return m_colorInfo.diffuseColor; }
	glm::vec3 getSpecularColor() const { return m_colorInfo.specularColor; }
	float getShininessFactor() const { return m_colorInfo.shininessFactor; }

	virtual bool intersecting(const Ray& ray, float& t) const = 0;
	virtual glm::vec3 getNormal(const glm::vec3 point) const = 0;
private:
	ColorInfo m_colorInfo;
};

