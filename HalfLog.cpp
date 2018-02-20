#pragma once
#include <GL/glew.h>
#include "Coord3.h"
#define _USE_MATH_DEFINES
#include "Coord8.h"
#include "utilOpenGL.h"
#include "HalfLog.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

using namespace std;

HalfLog::HalfLog(GLuint slices, GLfloat radius, GLfloat length, GLfloat cut, GLfloat cutThickness, GLuint whichSide, GLuint shaderID) :
	slices(slices), radius(radius), length(length), cut(cut), cutThickness(cutThickness), whichSide(whichSide)
{
	generate();
	circlesTexture = LoadMipmapTexture(GL_TEXTURE0, "iTreeTexture.png");
	glUniform1i(glGetUniformLocation(shaderID, "Texture0"), 0);
	interTexture = LoadMipmapTexture(GL_TEXTURE0, "interTexture.png");
	glUniform1i(glGetUniformLocation(shaderID, "Texture0"), 0);
	barkTexture = LoadMipmapTexture(GL_TEXTURE0, "oTreeTexture.png");
	glUniform1i(glGetUniformLocation(shaderID, "Texture0"), 0);
}

void HalfLog::addCircle() {
	GLuint offset = circlesVertices.size();
	GLfloat x = -length / 2.0f;
	GLfloat normX = -1.0f;
	GLfloat corner = 0.0f;
	GLfloat fz = cut - cutThickness / 2.0f;
	GLfloat alfa0 = (float)M_PI / 2.0f + (float)asin((- 2.0f * cut + cutThickness) / (2.0f * radius)); // cut < 0 for leftlog
	GLfloat theta = 2.0f * ((float)M_PI - alfa0) / (float)slices;
	GLfloat y, z, sa, ca;

	if (whichSide > 0) { // rightlog, cut > 0
		fz = cut + cutThickness / 2.0f;
		alfa0 = (float)acos((2.0f * cut + cutThickness) / (2.0f * radius));
		theta = -2.0f * alfa0 / (float)slices;
	}

	if (offset > 0) {
		x = -x;
		normX = -normX;
		corner = 1.0f;
	}

	GLfloat h = (float)sqrt(radius * radius - fz * fz);
	GLfloat alfa = alfa0;
	GLfloat multi = 1.0f / (2.0f * radius);

	circlesVertices.push_back(Coord8(x, 0.0f, fz,		 0.5f, 0.5f + fz * multi,		normX, 0.0f, 0.0f)); //midpoint
	barkVertices.push_back(Coord8(x, 0.0f, fz,			 0.5f, 0.5f + fz * multi,		normX, 0.0f, 0.0f));

	for (GLuint i = 1; i < slices + 2; ++i, alfa += theta) { //&& alfa <= 2.0f * alfa0
		sa = sin(alfa);
		ca = cos(alfa);
		z = radius * ca;
		y = radius * sa;
		circlesVertices.push_back(Coord8(x, y, z,	multi * (y + radius), multi * (z + radius),		normX, sa, ca));
		barkVertices.push_back(Coord8(x, y, z,		radius * alfa, corner,							normX, sa, ca));
		
		circlesIndices.push_back(Coord3(offset, i + offset, i + offset + 1));
	}
	circlesIndices.pop_back();
}

GLuint HalfLog::generateCircles() {
	addCircle();
	addCircle();
	return generateVAO(&circlesVertices, &circlesIndices);
}

GLuint HalfLog::generateBark() {
	GLuint offset = slices + 2; // 2 midpoints
	for (GLuint i = 1; i < slices; ++i) {
		barkIndices.push_back(Coord3(i, i + 1, i + offset));
		barkIndices.push_back(Coord3(i + 1, i + offset, i + offset + 1));
	}
	barkIndices.push_back(Coord3(slices, slices + 1, slices + offset + 1));
	barkIndices.push_back(Coord3(slices, slices + offset, slices + offset + 1));

	return generateVAO(&barkVertices, &barkIndices);
}

GLuint HalfLog::generateInterior() {
	GLfloat fz;
	if (whichSide > 0) // rightlog, cut > 0
		fz = cut + cutThickness / 2.0f;
	else
		fz = cut - cutThickness / 2.0f;

	GLfloat h = (float)sqrt(radius * radius - fz * fz);
	GLfloat normZ = 1.0f;
	if (whichSide > 0)
		normZ = -normZ;
	interVertices.push_back(Coord8(-length / 2.0f, h, fz, 0.0f, 1.0f, -1.0f, 0.0f, normZ)); //first point in left hCircle
	interVertices.push_back(Coord8(-length / 2.0f, -h, fz, 0.0f, 0.0f, -1.0f, 0.0f, normZ)); //last point in left hCircle
	interVertices.push_back(Coord8(length / 2.0f, h, fz, 1.0f, 1.0f, 1.0f, 0.0f, normZ)); //first point in right hCircle
	interVertices.push_back(Coord8(length / 2.0f, -h, fz, 1.0f, 0.0f, 1.0f, 0.0f, normZ)); //last point in right hCircle

	if (interIndices.size() == 0) {
		interIndices.push_back(Coord3(0, 1, 2));
		interIndices.push_back(Coord3(1, 2, 3));
	}
	return generateVAO(&interVertices, &interIndices);
}


void HalfLog::generate() {
	circlesVertices.clear();
	interVertices.clear();
	barkVertices.clear();
	circVAO = generateCircles();
	interVAO = generateInterior();
	barkVAO = generateBark();
}

void HalfLog::show() {
	glBindTexture(GL_TEXTURE_2D, circlesTexture);
	glBindVertexArray(circVAO);
	glDrawElements(GL_TRIANGLES, circlesIndices.size() * 3, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, interTexture);
	glBindVertexArray(interVAO);
	glDrawElements(GL_TRIANGLES, interIndices.size() * 3, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, barkTexture);
	glBindVertexArray(barkVAO);
	glDrawElements(GL_TRIANGLES, barkIndices.size() * 3, GL_UNSIGNED_INT, 0);
}