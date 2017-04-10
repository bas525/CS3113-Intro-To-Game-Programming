#pragma once
//Header file for Vector class
class Vect {
public:
	float x;
	float y;
	float z;

	Vect();

	Vect(float xp, float yp, float zp);

	float length() const;
};
