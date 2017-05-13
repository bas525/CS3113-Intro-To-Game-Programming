#include "Player.h"

std::vector<Laser> lasers;
Sword sword;

Player::Player() {
	width = 0;
	height = 0;
	rotation = 0;
	speed = 0;
	texture = 0;
	size = 0;
	aspect = 0;
	laserStock = 0;
	laserCooldown = 0;
	isLaserCooldown = false;
}

Player::Player(GLuint *spritep)
{
	position = Vect(0, -3, 0);
	width = 112.0f;
	height = 75.0f;
	scale = Vect(1, 1, 1);
	rotation = 0;
	speed = 3;
	texture = 0;
	updateMatrix();
	float playerSpriteX = 0.0f;
	float playerSpriteY = 941.0f;
	
	size = .4;
	SheetSprite playerSprite(spritep, playerSpriteX / 1024.0f, playerSpriteY / 1024.0f, width / 1024.0f, height / 1024.0f, size);
	//sprite = *spritep;
	sprite = playerSprite;
	aspect = width / height;

	laserCooldown = 0;
	isLaserCooldown = false;

	laserStock = 100;
	for (int i = 0; i < 3; i++) {
		lasers.push_back(Laser(spritep));
	}
	sword = *new Sword(spritep);
}

bool Player::fireLaser()
{	
	if (isLaserCooldown) {
		return false;
	}
	for (Laser &laser : lasers) {
		//std::cout << "fire" << std::endl;
		if (!laser.active) {
			laser.turnOn(position);
			isLaserCooldown = true;
			laserCooldown = .5;
			return true;
		}
	}

	return false;
}

void Player::drawSprite(ShaderProgram & program)
{
	updateMatrix();
	program.setModelMatrix(matrix);
	sprite.drawSprite(&program);
}

void Player::drawLasers(ShaderProgram & program)
{
	for (Laser &laser : lasers) {
		//std::cout << "fire" << std::endl;
		if (laser.active) {
			laser.drawSprite(program);
		}
	}
}

void Player::drawSword(ShaderProgram &program) {
	if (sword.active) {
		sword.drawSprite(program);
	}
}

std::vector<std::vector<Vect>> Player::laserCoordinates()
{
	std::vector<std::vector<Vect>> toReturn;
	for (Laser &laser : lasers) {
		//std::cout << "fire" << std::endl;
		toReturn.push_back(laser.globPoints());
	}

	return toReturn;
}

std::vector<bool> Player::activeLasers()
{
	std::vector<bool> toReturn;
	for (Laser &laser : lasers) {
		//std::cout << "fire" << std::endl;
		toReturn.push_back(laser.active);
	}

	return toReturn;
}
std::vector<bool> Player::dangerousLasers()
{
	std::vector<bool> toReturn;
	for (Laser &laser : lasers) {
		//std::cout << "fire" << std::endl;
		toReturn.push_back(laser.hurtUser);
	}

	return toReturn;
}

void Player::setLaserOff(int i)
{
	lasers[i].laserReset();
	lasers[i].turnOff(position);
	lasers[i].position = position;
	
}

void Player::laserBounce(int i)
{
	lasers[i].bounceBack();
}

void Player::resetAllLasers()
{
	for (Laser &laser : lasers) {
		laser.laserReset();
	}
}

std::vector<float> Player::getAllLaserX() {
	std::vector<float> toReturn;
	for (Laser &laser : lasers) {
		if (laser.active) {
			toReturn.push_back(laser.position.x);
		}
	}
	return toReturn;
}

void Player::resetSword()
{
	sword.resetSword();
}

void Player::swordAttack() {
	sword.turnOn(position);
}

std::vector<Vect> Player::swordCord() {
	return sword.globPoints();
}

bool Player::swordActive() {
	return sword.active;
}

bool Player::swordCanPlaySound() {
	return sword.canPlaySound();
}

void Player::updateMatrix() {
	matrix.identity();
	matrix.Translate(position.x, position.y, 0);
	matrix.Rotate(rotation*3.1415926 / 180);
	matrix.Scale(scale.x, scale.y, 0);
}

void Player::Input(float moveX, float moveY, float elapsed) {
	position.x += moveX * speed * elapsed;
	if (position.x < -3.55) position.x = -3.55;
	if (position.x > 3.55) position.x = 3.55;
	position.y += moveY * speed * elapsed;
	if (position.y < -4) position.y = -4;
	if (position.y > 4) position.y = 4;
	updateMatrix();
}

void Player::Update(float elapsed) {
	updateMatrix();
	for (Laser &laser : lasers) {
		laser.Update(elapsed);
	}
	sword.Update(elapsed, position);
	if (isLaserCooldown) {
		laserCooldown -= elapsed;
		if (laserCooldown < 0) {
			isLaserCooldown = false;
		}
	}
}

std::vector<Vect> Player::relPoints() {
	std::vector<Vect> points;
	/*points.push_back(Vect(-.5, .5, 0));
	points.push_back(Vect(.5, .5, 0));
	points.push_back(Vect(.5, -.5, 0));
	points.push_back(Vect(-.5, -.5, 0));*/
	//points.push_back(Vect(-.5*size*aspect, .5*size, 0));
	//points.push_back(Vect(.5*size*aspect, .5*size, 0));
	points.push_back(Vect(-.5*size*aspect, -.1*size, 0));
	points.push_back(Vect(0, .5*size, 0));
	points.push_back(Vect(.5*size*aspect, -.1*size, 0));
	points.push_back(Vect(.5*size*aspect, -.5*size, 0));
	points.push_back(Vect(-.5*size*aspect, -.5*size, 0));
	return points;
}

//returns the global corners of this object
std::vector<Vect> Player::globPoints() {
	std::vector<Vect> points = relPoints();
	for (Vect& v : points) {
		v = matrix * v;
	}
	return points;
}

Player::~Player()
{
}
