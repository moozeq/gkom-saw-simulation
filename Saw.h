#pragma once
#include <GL/glew.h>
#include "Coord3.h"
#include "Coord8.h"
#include <vector>

class Saw {
	GLuint sawVAO;
	GLuint texture;
	GLuint teethNum;
	GLfloat thickness;
	GLfloat teethHeight;
	GLfloat width;
	GLfloat height;
	
	std::vector<Coord8> vertices;
	std::vector<Coord3> indices;

	GLuint generate();
	void addLayer();
	void mergeLayers();

public:
	Saw(GLuint teethNum, GLfloat thickness, GLfloat teethHeight, GLfloat width, GLfloat height, GLuint shaderID);
	void show();

	GLuint getVAO() {
		return sawVAO;
	}
};