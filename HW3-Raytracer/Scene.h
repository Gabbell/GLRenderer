#pragma once

#include <fstream>
#include <vector>

#include "Camera.h"
#include "SceneObject.h"
#include "Light.h"

using namespace std;

class Scene
{
public:
	Scene(string sceneFile);
	~Scene();
	
	void parseSceneFile(ifstream& sceneFile);

	Camera* getCamera() const { return m_camera; }
	std::vector<SceneObject*> getObjects() const { return m_objects; }
	std::vector<Light*> getLights() const { return m_lights; }

	glm::vec3 getVec3(ifstream& inputStream);
	float getSingleParam(ifstream& inputStream);

private:
	void loadCamera();
	void loadSphere();
	void loadModel();
	void loadPlane();
	void loadLight();
	void loadTriangle();

	void meshToTriangles(string fileName, ColorInfo cinfo);
	ColorInfo parseColorInfo(glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shi);

	int m_numberOfObjects;
	ifstream m_sceneFile;

	Camera* m_camera = nullptr;
	std::vector<SceneObject*> m_objects;
	std::vector<Light*> m_lights;
};

