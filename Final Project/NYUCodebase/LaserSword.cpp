#include "LaserSword.h"



LaserSword::LaserSword()
{
	width = 0;
	height = 0;
	rotation = 0;
	speed = 0;
	texture = 0;
	size = 0;
	aspect = 0;
	direction = 0;
}

LaserSword::LaserSword(GLuint * spritep)
{
	position = Vect(0, -3, 0);
	width = 9.0f;
	height = 54.0f;
	scale = Vect(1, 1, 1);
	rotation = 0;
	speed = 5;
	texture = 0;
	updateMatrix();
	float laserSpriteX = 858.0f;
	float laserSpriteY = 230.0f;

	size = .5;
	SheetSprite laserSprite(spritep, laserSpriteX / 1024.0f, laserSpriteY / 1024.0f, width / 1024.0f, height / 1024.0f, size);
	//sprite = *spritep;
	sprite = laserSprite;
	aspect = width / height;

	active = false;
	direction = 1;
	hurtUser = false;
}

LaserSword::~LaserSword()
{
}
