#include "Enemy.h"



Enemy::Enemy()
{
	width = 0;
	height = 0;
	rotation = 0;
	speed = 0;
	texture = 0;
	size = 0;
	active = false;
}

Enemy::Enemy(float x, float y, GLuint * spritep)
{
	position = Vect(x, y, 0);
	width = 40.0f;
	height = 40.0f;
	scale = Vect(1, 1, 1);
	rotation = 0;
	speed = .9;
	texture = 0;
	updateMatrix();
	float enemySpriteX = 734.0f;
	float enemySpriteY = 977.0f;

	size = .3;
	SheetSprite enemySprite(spritep, enemySpriteX / 1024.0f, enemySpriteY / 1024.0f, width / 1024.0f, height / 1024.0f, size);
	sprite = enemySprite;
	aspect = width / height;
	active = true;
}

void Enemy::drawSprite(ShaderProgram & program)
{
	updateMatrix();
	program.setModelMatrix(matrix);
	sprite.drawSprite(&program);
}

void Enemy::updateMatrix()
{
	matrix.identity();
	matrix.Translate(position.x, position.y, 0);
	matrix.Rotate(rotation*3.1415926 / 180);
	matrix.Scale(scale.x, scale.y, 0);
}



void Enemy::Update(float x, float y, float elapsed)
{
	position.x += x*speed*elapsed;
	position.y += y*speed*elapsed;
}

void Enemy::moveX(float x) {
	position.x = x;
}

void Enemy::Die()
{
	active = false;
}

std::vector<Vect> Enemy::relPoints()
{
	std::vector<Vect> points;
	points.push_back(Vect(-.5*size*aspect, .5*size, 0));
	points.push_back(Vect(.5*size*aspect, .5*size, 0));
	points.push_back(Vect(.5*size*aspect, -.5*size, 0));
	points.push_back(Vect(-.5*size*aspect, -.5*size, 0));
	return points;
}

float Enemy::enemyLeft() {
	return position.x - .5*size*aspect;
}

float Enemy::enemyRight() {
	return position.x + .5*size*aspect;
}

float Enemy::halfWidth() {
	return .5*size*aspect;
}

std::vector<Vect> Enemy::globPoints()
{
	std::vector<Vect> points = relPoints();
	for (Vect& v : points) {
		v = matrix * v;
	}
	return points;
}


Enemy::~Enemy()
{
}
