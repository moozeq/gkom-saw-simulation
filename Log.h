#pragma once
#include <GL/glew.h>
#include "Coord3.h"
#include "Coord8.h"
#include <vector>

class Log {
	GLuint circVAO;
	GLuint barkVAO;
	GLuint circlesTexture;
	GLuint barkTexture;
	GLuint slices;
	GLfloat radius;
	GLfloat length;
	GLuint off;
	std::vector<Coord8> circlesVertices;
	std::vector<Coord8> barkVertices;
	std::vector<Coord3> circlesIndices;
	std::vector<Coord3> barkIndices;

	GLuint generateCircles();
	GLuint generateBark();
	void generate();
	void addCircle();

public:
	Log(GLuint slices, GLfloat radius, GLfloat length, GLuint shaderID);
	void show();

	GLuint getCircVAO() {
		return circVAO;
	}
	GLuint getBarkVAO() {
		return barkVAO;
	}
};
