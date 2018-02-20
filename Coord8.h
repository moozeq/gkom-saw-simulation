#pragma once

class Coord8
{
	GLfloat coords[8];
public:
	Coord8(GLfloat x, GLfloat y, GLfloat z, GLfloat s, GLfloat t, GLfloat nx, GLfloat ny, GLfloat nz) {
		coords[0] = x;
		coords[1] = y;
		coords[2] = z;
		coords[3] = s;
		coords[4] = t;
		coords[5] = nx;
		coords[6] = ny;
		coords[7] = nz;
	}
	GLfloat* getCoords() {
		return coords;
	}
};