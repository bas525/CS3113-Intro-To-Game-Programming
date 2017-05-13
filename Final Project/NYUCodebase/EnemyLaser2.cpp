#include "EnemyLaser2.h"



EnemyLaser2::EnemyLaser2()
{
}

EnemyLaser2::EnemyLaser2(GLuint * spritep)
{
	position = Vect(0, 0, 0);
	width = 41.0f;
	height = 41.0f;
	scale = Vect(1, 1, 1);
	rotation = 0;
	speed = 4.5;
	texture = 0;
	updateMatrix();
	float enemyLaserSpriteX = 310.0f;
	float enemyLaserSpriteY = 982.0f;

	size = .2;
	SheetSprite laserSprite(spritep, enemyLaserSpriteX / 1024.0f, enemyLaserSpriteY / 1024.0f, width / 1024.0f, height / 1024.0f, size);
	//sprite = *spritep;
	sprite = laserSprite;
	aspect = width / height;

	active = false;
	direction = -1;

	hurtUser = false;
}

void EnemyLaser2::bounceBack() {
	hurtUser = true;
	direction = 1;
}

void EnemyLaser2::damagedSelf() {
	hurtUser = false;
	direction = -1;
	active = false;
}

void EnemyLaser2::Update(float elapsed) {
	if (active) {
		position.y += elapsed*speed*direction;
		//std::cout << position.y << std::endl;
		if (position.y <= -4.1) {
			active = false;
		}
		if (position.y >= 4.1) {
			active = false;
		}

	}
	//std::cout << std::endl;

}
EnemyLaser2::~EnemyLaser2()
{
}
