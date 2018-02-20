#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class ShaderProgram
{
	GLuint shID; 	// The program ID
	GLuint lightColorLoc;
	GLuint lightPosLoc;
	GLuint viewPosLoc;
	GLuint viewLoc;
	GLuint projectionLoc;
	GLuint moveLoc;
	GLuint specLoc;

public:
	// Constructor reads and builds the shader
	ShaderProgram(const GLchar* vertexPath, const GLchar* fragmentPath);

	// Use the program
	void Use() const
	{
		glUseProgram(getID());
	}

	// returns program ID
	GLuint getID() const
	{
		return shID;
	}
	void setLightColor(glm::vec3 lightColor) {
		glUniform3fv(lightColorLoc, 1, &lightColor[0]);
	}
	void setLightPosition(glm::vec3 lightPosition) {
		glUniform3fv(lightPosLoc, 1, &lightPosition[0]);
	}
	void setViewPosition(glm::vec3 viewPosition) {
		glUniform3fv(viewPosLoc, 1, &viewPosition[0]);
	}
	void setView(glm::mat4 view) {
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}
	void setProjection(glm::mat4 projection) {
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	}
	void setMove(glm::mat4 move) {
		glUniformMatrix4fv(moveLoc, 1, GL_FALSE, glm::value_ptr(move));
	}
	void setSpecularStrength(GLfloat specStrength) {
		glUniform1f(specLoc, specStrength);
	}
};
