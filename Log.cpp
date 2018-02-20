#pragma once
#include <GL/glew.h>
#include "Coord3.h"
#define _USE_MATH_DEFINES
#include "Coord8.h"
#include "utilOpenGL.h"
#include "Log.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

using namespace std;

//50, 0.01f, 0.18f, 0.05f
Log::Log(GLuint slices, GLfloat radius, GLfloat length, GLuint shaderID) :
	slices(slices), radius(radius), length(length)
{
	generate();
	circlesTexture = LoadMipmapTexture(GL_TEXTURE0, "iTreeTexture.png");
	glUniform1i(glGetUniformLocation(shaderID, "Texture0"), 0);
	barkTexture = LoadMipmapTexture(GL_TEXTURE0, "oTreeTexture.png");
	glUniform1i(glGetUniformLocation(shaderID, "Texture0"), 0);
}

void Log::addCircle() {
	GLuint offset = circlesVertices.size();
	GLfloat x = -length / 2.0f;
	GLfloat normX = -1.0f;
	GLfloat corner = 0.0f;
	if (offset > 0) {
		x = -x;
		normX = -normX;
		corner = 1.0f;
	}
	GLfloat theta = (2.0f * (float)M_PI) / (float)slices;
	GLfloat alfa = - (float)M_PI / 2.0f;
	GLfloat y, z, sa, ca;
	GLfloat multi = 1.0f / (2.0f * radius);
	circlesVertices.push_back(Coord8(x, 0.0f, 0.0f,		0.5f, 0.5f,			normX, 0.0f, 0.0f)); //midpoint
	barkVertices.push_back(Coord8(x, 0.0f, 0.0f,		0.5f, 0.5f,			normX, 0.0f, 0.0f));
	
	for (GLuint i = 1; i < slices + 2; ++i, alfa += theta) {
		sa = sin(alfa);
		ca = cos(alfa);
		z = radius * ca;
		y = radius * sa;
		circlesVertices.push_back(Coord8(x, y, z,			multi * (y + radius), multi * (z + radius),		normX, sa, ca));
		barkVertices.push_back(Coord8(x, y, z,				radius * alfa, corner,							normX, sa, ca));

		circlesIndices.push_back(Coord3(offset, i + offset, i + offset + 1));
		}
	circlesIndices.pop_back();
}

GLuint Log::generateCircles() {
	addCircle();
	addCircle();
	return generateVAO(&circlesVertices, &circlesIndices);
}

GLuint Log::generateBark() {
	GLuint offset = slices + 2; // 2 midpoints
	for (GLuint i = 1; i < slices; ++i) {
		barkIndices.push_back(Coord3(i, i + 1, i + offset));
		barkIndices.push_back(Coord3(i + 1, i + offset, i + offset + 1));
	}
	barkIndices.push_back(Coord3(slices, slices + 1, slices + offset + 1));
	barkIndices.push_back(Coord3(slices, slices + offset, slices + offset + 1));

	return generateVAO(&barkVertices, &barkIndices);
}


void Log::generate() {
	circVAO = generateCircles();
	barkVAO = generateBark();
}

void Log::show() {
	glBindTexture(GL_TEXTURE_2D, circlesTexture);
	glBindVertexArray(circVAO);
	glDrawElements(GL_TRIANGLES, circlesIndices.size() * 3, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, barkTexture);
	glBindVertexArray(barkVAO);
	glDrawElements(GL_TRIANGLES, barkIndices.size() * 3, GL_UNSIGNED_INT, 0);
}