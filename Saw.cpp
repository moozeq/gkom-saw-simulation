#pragma once
#include <GL/glew.h>
#include "Coord3.h"
#include "Coord8.h"
#include "utilOpenGL.h"
#include "Saw.h"
#include <GLFW/glfw3.h>
#include <vector>

using namespace std;

Saw::Saw(GLuint teethNum, GLfloat thickness, GLfloat teethHeight, GLfloat width, GLfloat height, GLuint shaderID) :
	teethNum(teethNum), thickness(thickness), teethHeight(teethHeight), width(width), height(height)
{
	sawVAO = generate();
	texture = LoadMipmapTexture(GL_TEXTURE0, "metalTexture.png");
	glUniform1i(glGetUniformLocation(shaderID, "Texture0"), 0);
}

void Saw::addLayer() {
	GLfloat x = -width / 2.0f;					//coord x for vert in line, saw's begin
	GLfloat gap = (2.0f*fabs(x)) / teethNum;	//gap between teeth
	GLfloat x2 = x + gap / 2;
	GLuint firstVert = vertices.size();
	GLuint lastVert = firstVert + 2 * teethNum; //last vert in teeth

	GLfloat z = thickness / 2.0f;
	GLfloat normX = 1.0f;
	GLfloat normZ = 1.0f;
	if (firstVert > 0) {
		normZ = -normZ;
		z -= thickness;
	}

	for (GLuint i = 0; i < teethNum; ++i, x += gap, x2 += gap) {
		normX = -normX;

		vertices.push_back(Coord8(x, teethHeight, z, x + width / 2.0f, teethHeight / (height + teethHeight), normX, -1.0f, normZ));
		vertices.push_back(Coord8(x2, 0.0f, z, x2 + width / 2.0f, 0.0f, 0.0f, -1.0f, normZ));

		indices.push_back(Coord3(2 * i + firstVert, 2 * i + firstVert + 1, 2 * i + firstVert + 2));
	}
	vertices.push_back(Coord8(x, teethHeight, z, 1.0f, teethHeight / (height + teethHeight), normX, -1.0f, normZ));

	//fill
	vertices.push_back(Coord8(-x, teethHeight + height, z, 0.0f, 1.0f, -1.0f, 1.0f, normZ));
	vertices.push_back(Coord8(x, teethHeight + height, z, 1.0f, 1.0f, 1.0f, 1.0f, normZ));

	indices.push_back(Coord3(firstVert, lastVert, lastVert + 1));
	indices.push_back(Coord3(lastVert, lastVert + 1, lastVert + 2));
}

void Saw::mergeLayers() {
	GLuint firstVert = 0;
	GLuint firstVert2 = vertices.size() / 2;

	GLuint lastVert = teethNum * 2;
	GLuint lastVert2 = firstVert2 + teethNum * 2;

	//final fill sides
	indices.push_back(Coord3(firstVert, lastVert + 1, firstVert2));
	indices.push_back(Coord3(lastVert + 1, firstVert2, lastVert2 + 1));
	indices.push_back(Coord3(lastVert, lastVert + 2, lastVert2));
	indices.push_back(Coord3(lastVert + 2, lastVert2, lastVert2 + 2));

	//top
	indices.push_back(Coord3(lastVert + 1, lastVert + 2, lastVert2 + 1));
	indices.push_back(Coord3(lastVert + 2, lastVert2 + 1, lastVert2 + 2));

	//between teeth
	for (GLuint i = 0; i < teethNum * 2; i += 2) {
		indices.push_back(Coord3(i, i + lastVert + 3, i + firstVert2 + 1));
		indices.push_back(Coord3(i, i + 1, i + firstVert2 + 1));
		indices.push_back(Coord3(i + 2, i + 1, i + firstVert2 + 1));
		indices.push_back(Coord3(i + 2, i + firstVert2 + 2, i + firstVert2 + 1));
	}
}

GLuint Saw::generate() {
	addLayer();
	addLayer();
	mergeLayers();

	return generateVAO(&vertices, &indices);
}

void Saw::show() {
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(sawVAO);
	glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0);
}