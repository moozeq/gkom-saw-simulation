#define _USE_MATH_DEFINES
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shprogram.h"
#include "Coord3.h"
#include "Coord8.h"
#include "utilOpenGL.h"
#include "Saw.h"
#include "Log.h"
#include "HalfLog.h"
#include "Cuboid.h"
#include "Scene.h"
#include "Camera.h"

#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
using namespace std;

#define getrandomf(min, max) (min + ((max - min) * (rand() / (float)RAND_MAX)))
#define PARTNUM 200

#include <Windows.h>
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

const GLuint WIDTH = 1600, HEIGHT = 800;
const GLfloat logSpeed = 0.05f, hLogSpeed = 0.01f, sawSpeed = 5.0f;	//objects speed
const GLuint sawTeethNum = 150, logSlices = 80;							//how many details
const GLfloat sawThickness = 0.01f, sawTeethHeight = 0.05f, sawHeight = 1.2f, sawWidth = 0.2f; //saw's attributes
const GLfloat	logRadius = 0.1f, logLength = 0.8f;							//log's attributes
GLfloat deepness = 0.0f, logMoving = -1.2f, sawMoving = 0.0f, hLogMoving = 0.0f, sawPosition = 0.0f; //moving vars

Camera camera(glm::vec3(0.0f, 0.0f, 1.0f)); //camera's settings
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
GLboolean firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

GLboolean SAWSTOP = 0, GETLOG = 0, SHOWLOG = 1, SHOWHLOGS = 0, SAWMOVDIR = 1, HLOGSGEN = 0, SAWCHANGE = 0; //states
GLdouble time, sawTime = 0.0f; //stop/start saw

GLfloat particles[PARTNUM][8]; //x,y,z,angle0,rotX,rotY,rotZ,anglespeed
GLboolean isPart[PARTNUM][2];

void generateParticles() {
	for (int i = 0; i < PARTNUM; ++i) {
		particles[i][0] = getrandomf(-logLength / 2.0f, logLength / 2.0f);
		particles[i][1] = getrandomf(logRadius, logRadius + 0.01f);
		particles[i][2] = getrandomf(-0.05f, 0.05f);
		particles[i][3] = getrandomf(-(float)M_PI, (float)M_PI);
		particles[i][4] = getrandomf(0.0f, 1.0f);
		particles[i][5] = getrandomf(0.0f, 1.0f);
		particles[i][6] = getrandomf(0.0f, 1.0f);
		particles[i][7] = getrandomf(0.1f, 1.0f);
		isPart[i][0] = GL_FALSE;
	}
}

void showParticles(Cuboid* particle, ShaderProgram* shader) {
	for (int i = 0; i < PARTNUM; ++i) {
		if (!isPart[i][0]) { // not displaying all particles at once
			GLfloat is = getrandomf(0.0f, 1.0f);
			if (is > 0.9f) {
				isPart[i][0] = GL_TRUE;
				if (hLogMoving == 0.0f)
					isPart[i][1] = GL_FALSE;	//show particle when 1 log
				else
					isPart[i][1] = GL_TRUE;		//show particle when 2 hlogs
			}
			else
				continue;
		}
		glm::mat4 particleMove;

		particles[i][1] += getrandomf(0.001f, 0.02f); //y

		if (particles[i][2] > 0.0f)
			particles[i][2] += getrandomf(0.001f, 0.02f); //z
		else
			particles[i][2] -= getrandomf(0.001f, 0.02f); //z

		particles[i][3] += particles[i][7];

		if (isPart[i][1]) //particle w/ hlog speed
			particleMove = glm::translate(particleMove, glm::vec3(particles[i][0] + hLogMoving, particles[i][1], particles[i][2])) *
				glm::rotate(particleMove, particles[i][3], glm::vec3(particles[i][4], particles[i][5], particles[i][6]));
		else			  //particle w/o hlog speed
			particleMove = glm::translate(particleMove, glm::vec3(particles[i][0], particles[i][1], particles[i][2])) *
				glm::rotate(particleMove, particles[i][3], glm::vec3(particles[i][4], particles[i][5], particles[i][6]));

		
		switch (i % 3) {
		case 0:
			particleMove = glm::scale(particleMove, glm::vec3(1.0f, 1.0f, 1.0f));
			break;
		case 1:
			particleMove = glm::scale(particleMove, glm::vec3(1.5f, 1.5f, 1.5f));
			break;
		case 2:
			particleMove = glm::scale(particleMove, glm::vec3(0.5f, 0.5f, 0.5f));
			break;
		}
		shader->setMove(particleMove);
		particle->show();
	}
}

void calcSawPos(GLFWwindow *window) {
	if (!SAWSTOP) {	//saw's moving
		if (SAWCHANGE) { //saw's just started
			glfwSetTime(time);
			SAWCHANGE = 0;
		}
		else			//saw was moving b4
			time = glfwGetTime();
		deepness = -logRadius - 0.03f + ((float)sin(4 * time) + 1.0f) * (logRadius + 0.035f);
		sawMoving = sawSpeed * (float)sin(32 * time) / 100.0f;
	}
	else				//saw's stopped
		SAWCHANGE = 1;
}

void showLegsAndBars(Cuboid* leg, Cuboid* bar, ShaderProgram* shader) {
	glm::mat4 leg1, leg2, leg3, leg4, bar1, bar2;

	leg1 = glm::translate(leg1, glm::vec3(0.3f, deepness - sawTeethHeight, -0.2f));
	shader->setMove(leg1);
	leg->show();
	leg2 = glm::translate(leg2, glm::vec3(0.3f, deepness - sawTeethHeight, 0.2f));
	shader->setMove(leg2);
	leg->show();
	leg3 = glm::translate(leg3, glm::vec3(-0.3f, deepness - sawTeethHeight, 0.2f));
	shader->setMove(leg3);
	leg->show();
	leg4 = glm::translate(leg4, glm::vec3(-0.3f, deepness - sawTeethHeight, -0.2f));
	shader->setMove(leg4);
	leg->show();

	bar1 = glm::translate(bar1, glm::vec3(0.3f, deepness + 0.275f, 0.0f));
	shader->setMove(bar1);
	bar->show();
	bar2 = glm::translate(bar2, glm::vec3(-0.3f, deepness + 0.275f, 0.0f));
	shader->setMove(bar2);
	bar->show();
}

void showScene(Cuboid* stand, Cuboid* leg, Cuboid* bar, Scene* background, glm::mat4* standMove, glm::mat4* backgrMove, ShaderProgram* shader) {
	shader->setMove(*backgrMove);
	background->show();

	shader->setMove(*standMove);
	stand->show();

	showLegsAndBars(leg, bar, shader);
}

void showLog(Log* log, HalfLog* hlLog, HalfLog* hrLog, Cuboid* particle, ShaderProgram* shader) {
	glm::mat4 logMove;
	glm::mat4 hlLogMove;
	glm::mat4 hrLogMove;

	if (SHOWLOG) { //one log b4 cutting
		logMove = glm::translate(logMove, glm::vec3(logMoving, 0.0f, 0.0f));
		shader->setMove(logMove);
		log->show();

		if (GETLOG && logMoving < 0.0f) { //log's moving
			logMoving += logSpeed;
		}
		else if (GETLOG && logMoving >= 0.0f) { //log's cut, change to 2 hlogs
				if (deepness <= -logRadius) {
					SHOWLOG = 0;
					SHOWHLOGS = 1;
					hLogMoving = logMoving;
				}
				if (deepness < logRadius && !SAWSTOP)
					showParticles(particle, shader);
		}
	}
	if (SHOWHLOGS) { //2 halflogs after cutting
		static GLfloat gap = 0.0f; //gap between them, to show what's inside
		showParticles(particle, shader);
		if (!HLOGSGEN) {
			hlLog->setCut(sawPosition);
			hlLog->generate();
			hrLog->setCut(sawPosition);
			hrLog->generate();
			HLOGSGEN = 1;
		}

		glm::vec3 rotlLog = glm::vec3(1.0f, -logRadius, sawThickness / 2.0f); //slightly rotate hlogs
		hlLogMove = glm::translate(hlLogMove, glm::vec3(hLogMoving, 0.0f, -gap)) * glm::rotate(hlLogMove, -glm::radians(5.0f), rotlLog);
		shader->setMove(hlLogMove);
		hlLog->show();


		glm::vec3 rotrLog = glm::vec3(1.0f, -logRadius, sawThickness / 2.0f);
		hrLogMove = glm::translate(hrLogMove, glm::vec3(hLogMoving, 0.0f, gap)) * glm::rotate(hrLogMove, glm::radians(5.0f), rotrLog);
		shader->setMove(hrLogMove);
		hrLog->show();

		gap += 0.0005f;
		hLogMoving += hLogSpeed;
		if (hLogMoving > 2.0f) { //return, hide hlogs, show one log
			logMoving = -1.2f;
			hLogMoving = 0.0f;
			GETLOG = 0;
			SHOWLOG = 1;
			SHOWHLOGS = 0;
			HLOGSGEN = 0;
			gap = 0.0f;
			generateParticles();
		}
	}
}

void showSaw(Saw* saw, ShaderProgram* shader) {
	glm::mat4 sawMove;
	sawMove = glm::translate(sawMove, glm::vec3(sawMoving, deepness + 0.01f, sawPosition));
	shader->setMove(sawMove);
	shader->setSpecularStrength(5.0f);	//metal's high specular light value
	saw->show();
	shader->setSpecularStrength(0.0f);
}

int main()
{
	if (glfwInit() != GL_TRUE)
	{
		cout << "GLFW initialization failed" << endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	try
	{
		GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "[GKOM] - OpenGL", nullptr, nullptr);
		if (window == nullptr)
			throw exception("GLFW window not created");
		glfwMakeContextCurrent(window);

		glfwSetKeyCallback(window, key_callback);							//stop saw, get log
		glfwSetScrollCallback(window, scroll_callback);						//camera zoom
		glfwSetCursorPosCallback(window, mouse_callback);					//camera position
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  //resizing window
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);		//hide cursor

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
			throw exception("GLEW Initialization failed");

		glViewport(0, 0, WIDTH, HEIGHT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);

		ShaderProgram defShader("shader.vs", "shader.fs");
		Scene background(5.0f, 3.0f, 4.0f, defShader.getID());
		Saw saw(sawTeethNum, sawThickness, sawTeethHeight, sawHeight, sawWidth, defShader.getID());
		Log log(logSlices, logRadius, logLength, defShader.getID());
		HalfLog hlLog(logSlices / 2, logRadius, logLength, sawPosition, sawThickness, 0, defShader.getID()); //left log
		HalfLog hrLog(logSlices / 2, logRadius, logLength, sawPosition, sawThickness, 1, defShader.getID()); //right log
		Cuboid stand(2.4f, 0.3f, 1.2f, defShader.getID());
		Cuboid leg(0.05f, 0.6f, 0.05f, defShader.getID());
		Cuboid bar(0.05f, 0.05f, 0.45f, defShader.getID());
		Cuboid particle(0.05f, 0.01f, 0.01f, defShader.getID());
		particle.setTexture("interTexture.png", defShader.getID());
		glm::mat4 standMove;
		glm::mat4 backgrMove;
		glm::vec3 lightCol(1.0f, 1.0f, 1.0f);
		glm::vec3 lightPos(0.0f, -logRadius, -1.0f);

		standMove = glm::translate(standMove, glm::vec3(0.0f, -0.25f, 0.0f));
		backgrMove = glm::translate(backgrMove, glm::vec3(0.0f, 0.0f, 0.0f));

		srand((unsigned)glfwGetTime());
		generateParticles();

		while (!glfwWindowShouldClose(window))
		{
			GLfloat currentFrame = (float)glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			processInput(window);

			calcSawPos(window); // update saw pos & check if stopped
			glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			

			glm::mat4 view, projection;
			view = camera.GetViewMatrix();
			projection = glm::perspective(glm::radians(camera.zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

			defShader.Use();
			defShader.setLightColor(lightCol);
			defShader.setLightPosition(lightPos);
			defShader.setViewPosition(camera.position);
			defShader.setView(view);
			defShader.setProjection(projection);

			showScene(&stand, &leg, &bar, &background, &standMove, &backgrMove, &defShader);
			showLog(&log, &hlLog, &hrLog, &particle, &defShader);
			showSaw(&saw, &defShader);

			glBindVertexArray(0);
			glUseProgram(0);
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		glfwTerminate();
	}
	catch (exception ex)
	{
		cout << ex.what() << endl;
	}
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) { //stop saw
		if (SAWSTOP == 1)
			SAWSTOP = 0;
		else
			SAWSTOP = 1;
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) { //get log to cut
		if (GETLOG == 0 && deepness > logRadius)
			GETLOG = 1;
	}
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) //exit
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { //change saw position to cut log in diff shapes
		if (sawPosition > -logRadius + 0.03f)
			sawPosition -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		if (sawPosition < logRadius - 0.03f)
			sawPosition += 0.01f;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	GLfloat xoffset = (float)(xpos - lastX);
	GLfloat yoffset = (float)(lastY - ypos);

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.processMouseScroll((float)yoffset);
}