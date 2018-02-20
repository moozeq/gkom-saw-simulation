#pragma once
#include <GL/glew.h>
#include "Coord3.h"
#include "Coord8.h"
#include "utilOpenGL.h"
#include "Cuboid.h"
#include <GLFW/glfw3.h>
#include <vector>

using namespace std;

Cuboid::Cuboid(GLfloat width, GLfloat height, GLfloat depth, GLuint shaderID) :
	width(width), height(height), depth(depth)
{
	cubVAO = generate();
	texture = LoadMipmapTexture(GL_TEXTURE0, "woodTexture.png");
	glUniform1i(glGetUniformLocation(shaderID, "Texture0"), 0);
}

void Cuboid::addSides() {
	GLfloat x = width / 2.0f;
	GLfloat y = height / 2.0f;
	GLfloat z = depth / 2.0f;

	vertices.push_back(Coord8(x, y, -z, 1.0f, 1.0f,			1.0f, 1.0f, -1.0f));
	vertices.push_back(Coord8(x, y, z, 0.0f, 1.0f,			1.0f, 1.0f, 1.0f));
	vertices.push_back(Coord8(-x, y, z, 1.0f, 1.0f,			-1.0f, 1.0f, 1.0f));
	vertices.push_back(Coord8(-x, y, -z, 0.0f, 1.0f,		-1.0f, 1.0f, -1.0f));

	vertices.push_back(Coord8(x, -y, -z, 1.0f, 0.0f,		1.0f, -1.0f, -1.0f));
	vertices.push_back(Coord8(x, -y, z, 0.0f, 0.0f,			1.0f, -1.0f, 1.0f));
	vertices.push_back(Coord8(-x, -y, z, 1.0f, 0.0f,		-1.0f, -1.0f, 1.0f));
	vertices.push_back(Coord8(-x, -y, -z, 0.0f, 0.0f,		-1.0f, -1.0f, -1.0f));

	indices.push_back(Coord3(0, 1, 4)); //left & right
	indices.push_back(Coord3(1, 4, 5));
	indices.push_back(Coord3(2, 3, 6));
	indices.push_back(Coord3(3, 6, 7));

	indices.push_back(Coord3(1, 2, 5)); //front & back
	indices.push_back(Coord3(2, 5, 6));
	indices.push_back(Coord3(3, 0, 7));
	indices.push_back(Coord3(0, 7, 4));
}

void Cuboid::addTopBot() {
	GLfloat x = width / 2.0f;
	GLfloat y = height / 2.0f;
	GLfloat z = depth / 2.0f;
	GLuint offset = vertices.size();

	vertices.push_back(Coord8(x, y, -z, 1.0f, 1.0f,		1.0f, 1.0f, -1.0f));
	vertices.push_back(Coord8(x, y, z, 1.0f, 0.0f,		1.0f, 1.0f, 1.0f));
	vertices.push_back(Coord8(-x, y, z, 0.0f, 0.0f,		-1.0f, 1.0f, 1.0f));
	vertices.push_back(Coord8(-x, y, -z, 0.0f, 1.0f,	-1.0f, 1.0f, -1.0f));

	vertices.push_back(Coord8(x, -y, -z, 1.0f, 1.0f,	1.0f, -1.0f, -1.0f));
	vertices.push_back(Coord8(x, -y, z, 1.0f, 0.0f,		1.0f, -1.0f, 1.0f));
	vertices.push_back(Coord8(-x, -y, z, 0.0f, 0.0f,	-1.0f, -1.0f, 1.0f));
	vertices.push_back(Coord8(-x, -y, -z, 0.0f, 1.0f,	-1.0f, -1.0f, -1.0f));

	indices.push_back(Coord3(0 + offset, 1 + offset, 2 + offset)); //up & down
	indices.push_back(Coord3(0 + offset, 2 + offset, 3 + offset));
	indices.push_back(Coord3(4 + offset, 5 + offset, 6 + offset));
	indices.push_back(Coord3(4 + offset, 6 + offset, 7 + offset));
}

GLuint Cuboid::generate() {
	addSides();
	addTopBot();

	return generateVAO(&vertices, &indices);
}

void Cuboid::show() {
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(cubVAO);
	glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0);
}