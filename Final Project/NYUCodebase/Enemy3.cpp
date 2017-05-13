#include "Enemy3.h"



Enemy3::Enemy3()
{
}

Enemy3::Enemy3(float x, float y, float speedp, GLuint * spritep)
{
	position = Vect(x, y, 0);
	width = 98.0f;
	height = 96.0f;
	scale = Vect(1, 1, 1);
	rotation = 0;
	speed = speedp;
	texture = 0;
	updateMatrix();
	float enemySpriteX = 327.0f;
	float enemySpriteY = 452.0f;

	size = 1;
	SheetSprite enemySprite(spritep, enemySpriteX / 1024.0f, enemySpriteY / 1024.0f, width / 1024.0f, height / 1024.0f, size);
	sprite = enemySprite;
	aspect = width / height;
	active = true;
}

void Enemy3::Update(float x, float y, float elapsed)
{
	position.x =x;
	position.y -= speed*elapsed;
	rotation += 90 * elapsed * speed;
}

std::vector<Vect> Enemy3::relPoints()
{
	std::vector<Vect> points;
	points.push_back(Vect(-.5*size*aspect*.673, .5*size*.333, 0));
	points.push_back(Vect(.5*size*aspect*.327, .5*size*.958, 0));
	points.push_back(Vect(.5*size*aspect*.959, .5*size*.25, 0));
	points.push_back(Vect(.5*size*aspect*.592, -.5*size*.854, 0));
	points.push_back(Vect(-.5*size*aspect*.408, -.5*size*.958, 0));
	return points;
}

std::vector<Vect> Enemy3::globPoints()
{
	std::vector<Vect> points = relPoints();
	for (Vect& v : points) {
		v = matrix * v;
	}
	return points;
}

Enemy3::~Enemy3()
{
}
