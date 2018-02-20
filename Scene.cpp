#pragma once
#include <GL/glew.h>
#include "Coord3.h"
#include "Coord8.h"
#include "utilOpenGL.h"
#include "Scene.h"
#include <GLFW/glfw3.h>
#include <vector>

using namespace std;

Scene::Scene(GLfloat width, GLfloat height, GLfloat depth, GLuint shaderID) :
	width(width), height(height), depth(depth)
{
	generate();
	sidesTexture = LoadMipmapTexture(GL_TEXTURE0, "backgroundTexture.png");
	glUniform1i(glGetUniformLocation(shaderID, "Texture0"), 0);

	topBotTexture = LoadMipmapTexture(GL_TEXTURE0, "floorTexture.png");
	glUniform1i(glGetUniformLocation(shaderID, "Texture0"), 0);
}

void Scene::addSides() {
	GLfloat x = width / 2.0f;
	GLfloat y = height / 2.0f;
	GLfloat z = depth / 2.0f;

	sidesVertices.push_back(Coord8(x, y, -z, 1.0f, 1.0f,			-1.0f, -1.0f, 1.0f));
	sidesVertices.push_back(Coord8(x, y, z, 0.0f, 1.0f,			-1.0f, -1.0f, -1.0f));
	sidesVertices.push_back(Coord8(-x, y, z, 1.0f, 1.0f,			1.0f, -1.0f, -1.0f));
	sidesVertices.push_back(Coord8(-x, y, -z, 0.0f, 1.0f,		1.0f, -1.0f, 1.0f));

	sidesVertices.push_back(Coord8(x, -y, -z, 1.0f, 0.0f,		-1.0f, 1.0f, 1.0f));
	sidesVertices.push_back(Coord8(x, -y, z, 0.0f, 0.0f,			-1.0f, 1.0f, -1.0f));
	sidesVertices.push_back(Coord8(-x, -y, z, 1.0f, 0.0f,		1.0f, 1.0f, -1.0f));
	sidesVertices.push_back(Coord8(-x, -y, -z, 0.0f, 0.0f,		1.0f, 1.0f, 1.0f));

	sidesIndices.push_back(Coord3(0, 1, 4)); //left & right
	sidesIndices.push_back(Coord3(1, 4, 5));
	sidesIndices.push_back(Coord3(2, 3, 6));
	sidesIndices.push_back(Coord3(3, 6, 7));

	sidesIndices.push_back(Coord3(1, 2, 5)); //front & back
	sidesIndices.push_back(Coord3(2, 5, 6));
	sidesIndices.push_back(Coord3(3, 0, 7));
	sidesIndices.push_back(Coord3(0, 7, 4));
}

void Scene::addTopBot() {
	GLfloat x = width / 2.0f;
	GLfloat y = height / 2.0f;
	GLfloat z = depth / 2.0f;

	topBotVertices.push_back(Coord8(x, y, -z, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f));
	topBotVertices.push_back(Coord8(x, y, z, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f));
	topBotVertices.push_back(Coord8(-x, y, z, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f));
	topBotVertices.push_back(Coord8(-x, y, -z, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f));

	topBotVertices.push_back(Coord8(x, -y, -z, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f));
	topBotVertices.push_back(Coord8(x, -y, z, 1.0f, 0.0f, -1.0f, 1.0f, -1.0f));
	topBotVertices.push_back(Coord8(-x, -y, z, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f));
	topBotVertices.push_back(Coord8(-x, -y, -z, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f));

	topBotIndices.push_back(Coord3(0, 1, 2)); //up & down
	topBotIndices.push_back(Coord3(0, 2, 3));
	topBotIndices.push_back(Coord3(4, 5, 6));
	topBotIndices.push_back(Coord3(4, 6, 7));
}


GLuint Scene::generateSides() {
	addSides();

	return generateVAO(&sidesVertices, &sidesIndices);
}

GLuint Scene::generateTopBot() {
	addTopBot();

	return generateVAO(&topBotVertices, &topBotIndices);
}

void Scene::generate() {
	sidesVAO = generateSides();
	topBotVAO = generateTopBot();
}

void Scene::show() {
	glBindTexture(GL_TEXTURE_2D, sidesTexture);
	glBindVertexArray(sidesVAO);
	glDrawElements(GL_TRIANGLES, sidesIndices.size() * 3, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, topBotTexture);
	glBindVertexArray(topBotVAO);
	glDrawElements(GL_TRIANGLES, topBotIndices.size() * 3, GL_UNSIGNED_INT, 0);
}