#include "EnemyLaser1.h"



EnemyLaser1::EnemyLaser1()
{
}

EnemyLaser1::EnemyLaser1(GLuint * spritep)
{
	position = Vect(0, 0, 0);
	width = 37.0f;
	height = 38.0f;
	scale = Vect(1, 1, 1);
	rotation = 0;
	speed = 4.5;
	texture = 0;
	updateMatrix();
	float enemyLaserSpriteX = 740.0f;
	float enemyLaserSpriteY = 686.0f;

	size = .2;
	SheetSprite laserSprite(spritep, enemyLaserSpriteX / 1024.0f, enemyLaserSpriteY / 1024.0f, width / 1024.0f, height / 1024.0f, size);
	//sprite = *spritep;
	sprite = laserSprite;
	aspect = width / height;

	active = false;
	direction = -1;
}

void EnemyLaser1::Update(float elapsed) {
	if (active) {
		position.y += elapsed*speed*direction;
		rotation += 250 * elapsed;
	}
	if (position.y <= -4.1) {
		active = false;
	}
}



EnemyLaser1::~EnemyLaser1()
{
}
