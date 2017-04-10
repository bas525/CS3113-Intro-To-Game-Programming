#include "Vect.h"
#include <math.h>
//cpp file for Vector class

Vect::Vect(){
	x = 0;
	y = 0;
	z = 0;
}


Vect::Vect(float xp, float yp, float zp) {
	x = xp;
	y = yp;
	z = zp;
}

float Vect::length() const {
	return sqrt(x*x + y*y + z*z);
}