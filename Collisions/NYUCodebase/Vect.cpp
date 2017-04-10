#include "Vect.h"
#include <math.h>
//cpp file for Vector class

Vect::Vect(){}


Vect::Vect(float xp, float yp, float zp) {
	x = xp;
	y = yp;
	z = zp;
}

float Vect::length() const {
	return sqrt(x*x + y*y + z*z);
}