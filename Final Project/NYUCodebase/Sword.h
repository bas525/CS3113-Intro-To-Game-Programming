#pragma once
#include "Laser.h"
class Sword 
{
public:
	Matrix matrix;
	Vect position;
	float width;
	float height;
	Vect scale;
	float rotation;
	float speed;
	GLuint texture;
	SheetSprite sprite;
	float size;
	float  aspect;

	bool active;
	float direction;
	bool hurtUser;
	float cooldown = 0;

	Sword();
	Sword(GLuint * spritep);
	void drawSprite(ShaderProgram & program);
	void turnOn(Vect pos);
	void turnOff();
	void Update(float elapsed, Vect pos);
	void updateMatrix();
	bool canPlaySound();
	std::vector<Vect> relPoints();
	std::vector<Vect> globPoints();
	void resetSword();
	~Sword();
};

