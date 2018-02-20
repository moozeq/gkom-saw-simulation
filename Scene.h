#pragma once
#include <GL/glew.h>
#include "Coord3.h"
#include "Coord8.h"
#include <vector>

class Scene {
	GLuint sidesVAO;
	GLuint topBotVAO;
	GLuint sidesTexture;
	GLuint topBotTexture;
	GLfloat width;
	GLfloat height;
	GLfloat depth;

	std::vector<Coord8> sidesVertices;
	std::vector<Coord8> topBotVertices;
	std::vector<Coord3> sidesIndices;
	std::vector<Coord3> topBotIndices;

	GLuint generateSides();
	GLuint generateTopBot();
	void generate();
	void addSides();
	void addTopBot();

public:
	Scene(GLfloat width, GLfloat height, GLfloat depth, GLuint shaderID);
	void show();

	GLuint getSidesVAO() {
		return sidesVAO;
	}

	GLuint getTopBotVAO() {
		return topBotVAO;
	}
};