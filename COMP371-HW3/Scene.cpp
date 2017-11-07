#include "Scene.h"

#include <string>
#include <iostream>

#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "objloader.hpp"

#include "glm/glm.hpp"

using namespace std;

Scene::Scene(string fileName) 
	:m_sceneFile(fileName, ios::in)
{
	// Opening scene file
	if (!m_sceneFile.is_open()) {
		std::cout << "Could not find specified file" << std::endl;
	}

	parseSceneFile(m_sceneFile);
}

void Scene::parseSceneFile(std::ifstream& sceneFile) {
	// Parsing scene file
	try {
		sceneFile >> m_numberOfObjects;
		while (!sceneFile.eof()) {
			string currentObject;
			string parameterName;
			sceneFile >> currentObject;

			if (currentObject == "camera") {
				loadCamera();
			}
			else if (currentObject == "sphere") {
				loadSphere();
			}
			else if (currentObject == "light") {
				loadLight();
			}
			else if (currentObject == "model") {
				loadModel();
			}
			else if (currentObject == "plane") {
				loadPlane();
			}
			else if (currentObject == "triangle") {
				loadTriangle();
			}
		}
	}
	catch (runtime_error& e) {
		cout << e.what() << endl;
	}
}

void Scene::loadCamera() {
	string parameter;
	glm::vec3 pos;					// Camera position
	float fov, f;					// Field of view and focal length
	float a;						// Aspect ratio
	for (int i = 0; i < 4; i++) {	// Camera has 4 parameters
		m_sceneFile >> parameter;
		if (parameter == "pos:") {
			pos = getVec3(m_sceneFile);
		}
		else if (parameter == "fov:") {
			fov = getSingleParam(m_sceneFile);
		}
		else if (parameter == "f:") {
			f = getSingleParam(m_sceneFile);
		}
		else if (parameter == "a:") {
			a = getSingleParam(m_sceneFile);
		}
		else cout << "Camera parameter " << parameter << " not recognized." << endl;
	}
	m_camera = new Camera(pos, glm::radians(fov), f, a);
}

void Scene::loadSphere() {
	string parameter;
	glm::vec3 pos, amb, dif, spe;	// Sphere position, ambient color, diffuse color and specular color
	int rad, shi;					// Sphere radius and shininess
	for (int i = 0; i < 6; i++) {	// Sphere has 6 parameters
		m_sceneFile >> parameter;
		if (parameter == "pos:") {
			pos = getVec3(m_sceneFile);
		}
		else if (parameter == "rad:") {
			rad = getSingleParam(m_sceneFile);
		}
		else if (parameter == "amb:") {
			amb = getVec3(m_sceneFile);
		}
		else if (parameter == "dif:") {
			dif = getVec3(m_sceneFile);
		}
		else if (parameter == "spe:") {
			spe = getVec3(m_sceneFile);
		}
		else if (parameter == "shi:") {
			shi = getSingleParam(m_sceneFile);
		}
		else cout << "Sphere parameter " << parameter << " not recognized." << endl;
	}
	ColorInfo cinfo = parseColorInfo(amb, dif, spe, shi);
	m_objects.push_back(new Sphere(pos, rad, cinfo));
}

void Scene::loadLight() {
	string parameter;
	glm::vec3 pos, col;				// Light position and color
	for (int i = 0; i < 2; i++) {	// Light has 2 parameters
		m_sceneFile >> parameter;
		if (parameter == "pos:") {
			pos = getVec3(m_sceneFile);
		}
		else if (parameter == "col:") {
			col = getVec3(m_sceneFile);
		}
		else cout << "Light parameter " << parameter << " not recognized." << endl;
	}
	m_lights.push_back(new Light(pos, col));
}

void Scene::loadModel() {
	string parameter;
	string modelName;				// Model .obj file name
	glm::vec3 amb, dif, spe;		// Ambient, diffuse and specular color of the model
	int shi;						// Specular shininess factor
	for (int i = 0; i < 5; i++) {
		m_sceneFile >> parameter;
		if (i == 0) {
			modelName = parameter;
		}
		else if (parameter == "amb:") {
			amb = getVec3(m_sceneFile);
		}
		else if (parameter == "dif:") {
			dif = getVec3(m_sceneFile);
		}
		else if (parameter == "spe:") {
			spe = getVec3(m_sceneFile);
		}
		else if (parameter == "shi:") {
			shi = getSingleParam(m_sceneFile);
		}
		else cout << "Model parameter " << parameter << " not recognized." << endl;
	}
	ColorInfo cinfo = parseColorInfo(amb, dif, spe, shi);
	meshToTriangles(modelName, cinfo);
}

void Scene::loadPlane() {
	string parameter;
	glm::vec3 nor, pos, amb, dif, spe;		// Ambient, diffuse and specular color of the plane
	int shi;								// Specular shininess factor
	for (int i = 0; i < 6; i++) {			// Plane has 6 parameters
		m_sceneFile >> parameter;
		if (parameter == "nor:") {
			nor = getVec3(m_sceneFile);
		}
		else if (parameter == "pos:") {
			pos = getVec3(m_sceneFile);
		}
		else if (parameter == "amb:") {
			amb = getVec3(m_sceneFile);
		}
		else if (parameter == "dif:") {
			dif = getVec3(m_sceneFile);
		}
		else if (parameter == "spe:") {
			spe = getVec3(m_sceneFile);
		}
		else if (parameter == "shi:") {
			shi = getSingleParam(m_sceneFile);
		}
		else cout << "Model parameter " << parameter << " not recognized." << endl;
	}
	ColorInfo cinfo = parseColorInfo(amb, dif, spe, shi);
	m_objects.push_back(new Plane(pos, nor, cinfo));
}

void Scene::loadTriangle() {
	string parameter;
	glm::vec3 v1, v2, v3, amb, dif, spe;
	int shi;
	for(int i = 0; i < 7; i++) {
		m_sceneFile >> parameter;
		if (parameter == "v1:") {
			v1 = getVec3(m_sceneFile);
		}
		else if (parameter == "v2:") {
			v2 = getVec3(m_sceneFile);
		}
		else if (parameter == "v3:") {
			v3 = getVec3(m_sceneFile);
		}
		else if (parameter == "amb:") {
			amb = getVec3(m_sceneFile);
		}
		else if (parameter == "dif:") {
			dif = getVec3(m_sceneFile);
		}
		else if (parameter == "spe:") {
			spe = getVec3(m_sceneFile);
		}
		else if (parameter == "shi:") {
			shi = getSingleParam(m_sceneFile);
		}
		else cout << "Triangle parameter " << parameter << " not recognized." << endl;
	}
	ColorInfo cinfo = parseColorInfo(amb, dif, spe, shi);
	m_objects.push_back(new Triangle(v1, v2, v3, cinfo));
}

void Scene::meshToTriangles(string fileName, ColorInfo cinfo) {
	std::vector<glm::vec3> vertices, normals;
	std::vector<glm::vec2> uvs;
	if (!loadOBJ(fileName.c_str(), vertices, normals, uvs)) {
		std::cout << "Could not load the .obj file" << std::endl;
	}
	else {
		for (int i = 0; i < vertices.size() - 2; i+=3) {
			glm::vec3 v1 = vertices.at(i);
			glm::vec3 v2 = vertices.at(i + 1);
			glm::vec3 v3 = vertices.at(i + 2);
			m_objects.push_back(new Triangle(v1, v2, v3, cinfo));
		}
	}
}

ColorInfo Scene::parseColorInfo(glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shi) {
	ColorInfo cinfo;
	cinfo.ambientColor = amb;
	cinfo.diffuseColor = dif;
	cinfo.specularColor = spe;
	cinfo.shininessFactor = shi;
	return cinfo;
}

glm::vec3 Scene::getVec3(ifstream& inputStream) {
	float x, y, z;
	inputStream >> x;
	inputStream >> y;
	inputStream >> z;
	return glm::vec3(x, y, z);
}

float Scene::getSingleParam(ifstream& inputStream) {
	float param;
	inputStream >> param;
	return param;
}

Scene::~Scene() {
	delete m_camera;
	m_sceneFile.clear();
	m_sceneFile.close();
}
