#include "Enemy2.h"



Enemy2::Enemy2()
{
	width = 0;
	height = 0;
	rotation = 0;
	speed = 0;
	texture = 0;
	size = 0;
	active = false;
}

Enemy2::Enemy2(float x, float y, GLuint * spritep)
{
	position = Vect(x, y, 0);
	width = 97.0f;
	height = 84.0f;
	scale = Vect(1, 1, 1);
	rotation = 0;
	speed = .9;
	texture = 0;
	updateMatrix();
	float enemySpriteX = 346.0f;
	float enemySpriteY = 150.0f;

	size = .8;
	SheetSprite enemySprite(spritep, enemySpriteX / 1024.0f, enemySpriteY / 1024.0f, width / 1024.0f, height / 1024.0f, size);
	sprite = enemySprite;
	aspect = width / height;
	active = true;
}


Enemy2::~Enemy2()
{
}
