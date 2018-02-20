#pragma once

class Coord3
{
	GLuint coords[3];
public:
	Coord3(GLuint A, GLuint B, GLuint C) {
		coords[0] = A;
		coords[1] = B;
		coords[2] = C;
	}
	GLuint* getCoords() {
		return coords;
	}
};