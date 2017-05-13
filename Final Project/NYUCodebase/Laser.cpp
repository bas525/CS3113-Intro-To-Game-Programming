#include "Laser.h"



Laser::Laser()
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

Laser::Laser(GLuint * spritep)
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

void Laser::drawSprite(ShaderProgram & program)
{
	updateMatrix();
	program.setModelMatrix(matrix);
	sprite.drawSprite(&program);
}

void Laser::updateMatrix() {
	matrix.identity();
	matrix.Translate(position.x, position.y, 0);
	matrix.Rotate(rotation*3.1415926 / 180);
	matrix.Scale(scale.x, scale.y, 0);
}

void Laser::turnOn(Vect pos)
{
	position = pos;
	active = true;
}

void Laser::turnOff(Vect pos)
{
	position = pos;
	active = false;
	hurtUser = false;
}

void Laser::bounceBack() {
	hurtUser = true;
	direction = -1;
	rotation = 180;
}

void Laser::laserReset() {
	direction = 1;
	rotation = 0;
	hurtUser = false;
	active = false;
	position = Vect(0, 0, 0);
}

void Laser::Update(float elapsed) {
	if (active) {
		position.y += elapsed*speed*direction;
	}
	if (position.y >= 4.1) {
		hurtUser = false;
		active = false;
		direction = 1;
		rotation = 0;
	}
	if (position.y <= -4.1) {
		hurtUser = false;
		active = false;
		direction = 1;
		rotation = 0;
	}
}

std::vector<Vect> Laser::relPoints() {
	std::vector<Vect> points;
	/*points.push_back(Vect(-.5, .5, 0));
	points.push_back(Vect(.5, .5, 0));
	points.push_back(Vect(.5, -.5, 0));
	points.push_back(Vect(-.5, -.5, 0));*/
	points.push_back(Vect(-.5*size*aspect, .5*size, 0));
	points.push_back(Vect(.5*size*aspect, .5*size, 0));
	points.push_back(Vect(.5*size*aspect, -.5*size, 0));
	points.push_back(Vect(-.5*size*aspect, -.5*size, 0));
	return points;
}

//returns the global corners of this object
std::vector<Vect> Laser::globPoints() {
	std::vector<Vect> points = relPoints();
	for (Vect& v : points) {
		v = matrix * v;
	}
	return points;
}

Laser::~Laser()
{

}
