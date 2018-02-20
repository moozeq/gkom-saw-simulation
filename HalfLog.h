#pragma once
#include <GL/glew.h>
#include "Coord3.h"
#include "Coord8.h"
#include <vector>

class HalfLog {
	GLuint circVAO;
	GLuint barkVAO;
	GLuint interVAO;
	GLuint circlesTexture;
	GLuint barkTexture;
	GLuint interTexture;
	GLuint slices;
	GLuint whichSide;
	GLfloat radius;
	GLfloat length;
	GLfloat cutThickness;
	GLfloat cut;

	std::vector<Coord8> circlesVertices;
	std::vector<Coord8> barkVertices;
	std::vector<Coord8> interVertices;
	std::vector<Coord3> circlesIndices;
	std::vector<Coord3> barkIndices;
	std::vector<Coord3> interIndices;

	GLuint generateCircles();
	GLuint generateBark();
	GLuint generateInterior();
	void addCircle();

public:
	HalfLog(GLuint slices, GLfloat radius, GLfloat length, GLfloat cut, GLfloat cutThickness, GLuint whichSide, GLuint shaderID);
	void show();
	void generate();

	GLuint getCircVAO() {
		return circVAO;
	}
	GLuint getBarkVAO() {
		return barkVAO;
	}
	GLuint getInterVAO() {
		return interVAO;
	}
	void setCut(GLfloat newCut) {
		cut = newCut;
	}
};
