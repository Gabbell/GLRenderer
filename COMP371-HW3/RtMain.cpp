// Standard Libraries
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <algorithm>

#include <iostream>

//External Headers
#include "CImg.h"
#include "glm\glm.hpp"

#include "Scene.h"

using namespace cimg_library;

//Default clear color;
unsigned char clearColor = 0;

// Scene & Camera
Scene* scene = nullptr;
const Camera* sceneCam = nullptr;

void raytrace(CImg<float>& imgData);
constexpr float clamp(float v, float min, float max);

/*
 * Ray-Tracing. The variable names are the same as specified in the scene file that is parsed for the sake of clarity.
 */

int main() {
	string sceneName;
	std::cout << "Please enter the name of the scene file" << std::endl;
	std::cin >> sceneName;

	try {
		scene = new Scene(sceneName);
	}
	catch (std::runtime_error& e) {
		std::cout << e.what() << std::endl;
		system("pause");
		return 1;
	}

	sceneCam = scene->getCamera();

	float theta = sceneCam->getFov() / 2.0f;
	float height = 2.0f * tan(theta) * sceneCam->getFocalLength();
	float width = height * sceneCam->getAR();
	CImg<float> imgData(width, height, 1, 3, 0);

	std::cout << "Image size of " << width << " x " << height << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~Starting Raytracing~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	raytrace(imgData);

	CImgDisplay display(imgData, "RayTracing");
	while (!display.is_closed()) {
		display.wait();
	}

	delete scene;

	return 0;
}

void raytrace(CImg<float>& imgData) {
	glm::vec3 cameraPosition = sceneCam->getPosition();

	float left = cameraPosition.x - (imgData.width() / 2.0f) + 0.5f;
	float top = cameraPosition.y + (imgData.height() / 2.0f) - 0.5f;

	for (int x = 0; x < imgData.width(); x++) {
		for (int y = 0; y < imgData.height(); y++) {
			Ray ray = { cameraPosition, glm::normalize(glm::vec3(left + x, top - y, cameraPosition.z - sceneCam->getFocalLength()) - cameraPosition) };

			const SceneObject* closestObject = nullptr;
			float minimumT = 0;
			for (const SceneObject* object : scene->getObjects()) {
				float t = 0;
				if (object->intersecting(ray, t)) {
					if (t > 0) {
						if (closestObject) {
							if (t < minimumT) {
								minimumT = t;
								closestObject = object;
							}
						}
						else {
							minimumT = t;
							closestObject = object;
						}
					}
				}
			}
			// An object has been hit
			if (closestObject) {
				glm::vec3 intersectPoint(ray.origin + (ray.direction * minimumT));

				glm::vec3 finalColor = closestObject->getAmbientColor();
				for (const Light* light : scene->getLights()) {
					// Cast a shadow ray
					Ray rayToLight = { intersectPoint, glm::normalize(light->getPosition() - intersectPoint) };

					bool isOccluded = false;
					float t = 0;
					for (const SceneObject* object : scene->getObjects()) {
						if (object == closestObject) continue ;
						if (object->intersecting(rayToLight, t)) {
							glm::vec3 hitPoint = (rayToLight.origin + rayToLight.direction*t);
							float distanceToLight = glm::length(light->getPosition() - intersectPoint);
							float distanceToObject = glm::length(hitPoint - intersectPoint);
							if (t > 0 && distanceToObject < distanceToLight) isOccluded = true ; 
						}
					}
					if (!isOccluded) {
						glm::vec3 normal = closestObject->getNormal(intersectPoint);
						glm::vec3 reflection = glm::reflect(-rayToLight.direction, normal);
						glm::vec3 diffuse = closestObject->getDiffuseColor() * std::max(glm::dot(rayToLight.direction, normal), 0.0f);
						glm::vec3 specular = closestObject->getSpecularColor() * pow(max(glm::dot(reflection, -ray.direction) , 0.0f), closestObject->getShininessFactor());
						finalColor += light->getColor() * (diffuse + specular);
					}
				}
				imgData(x, y, 0, 0) = clamp(finalColor.r, 0.0f, 1.0f);
				imgData(x, y, 0, 1) = clamp(finalColor.g, 0.0f, 1.0f);
				imgData(x, y, 0, 2) = clamp(finalColor.b, 0.0f, 1.0f);
			}
		}
	}
}

// Make sure the color is between 0 and 1
constexpr float clamp(float v, float min, float max) {
	return (v < min) ? min : ((v > max) ? max : v);
}