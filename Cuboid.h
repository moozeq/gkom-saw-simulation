#pragma once
#include <GL/glew.h>
#include "Coord3.h"
#include "Coord8.h"
#include <vector>
#include <string>

class Cuboid {
	GLuint cubVAO;
	GLuint texture;
	GLfloat width;
	GLfloat height;
	GLfloat depth;

	std::vector<Coord8> vertices;
	std::vector<Coord3> indices;

	GLuint generate();
	void addSides();
	void addTopBot();

public:
	Cuboid(GLfloat width, GLfloat height, GLfloat depth, GLuint shaderID);
	void show();
	void setTexture(string newTexture, GLuint shaderID) {
		texture = LoadMipmapTexture(GL_TEXTURE0, newTexture.c_str());
		glUniform1i(glGetUniformLocation(shaderID, "Texture0"), 0);
	}
	GLuint getVAO() {
		return cubVAO;
	}
};