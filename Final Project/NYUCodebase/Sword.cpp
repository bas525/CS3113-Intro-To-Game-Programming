#include "Sword.h"



Sword::Sword()
{
	width = 0;
	height = 0;
	rotation = 0;
	speed = 0;
	texture = 0;
	size = 0;
	aspect = 0;
	direction = 0;
	active = false;
}

Sword::Sword(GLuint * spritep)
{
	position = Vect(0, -3, 0);
	width = 9;
	height = 57.0f;
	scale = Vect(1, 1, 1);
	rotation = 90;
	speed = 500;
	texture = 0;
	updateMatrix();
	float laserSpriteX = 856;
	float laserSpriteY = 926;

	size = 1.5;
	SheetSprite laserSprite(spritep, laserSpriteX / 1024.0f, laserSpriteY / 1024.0f, width / 1024.0f, height / 1024.0f, size);
	//sprite = *spritep;
	sprite = laserSprite;
	aspect = width / height;

	active = false;
	direction = 1;
	hurtUser = false;
}
void Sword::drawSprite(ShaderProgram & program)
{
	updateMatrix();
	program.setModelMatrix(matrix);
	sprite.drawSprite(&program);
}
void Sword::turnOn(Vect pos) {
	if(cooldown <= 0) {
		position = pos;
		active = true;
	}
}

void Sword::turnOff() {
	active = false;
	rotation = 90;
}

void Sword::Update(float elapsed, Vect pos) {
	if (active) {
		cooldown = 1;
		position = pos;
		rotation += elapsed*speed;
		if (rotation >= 90+360) {
			turnOff();
		}
	}
	//std::cout << cooldown << std::endl;
	cooldown -= elapsed;
}

void Sword::updateMatrix() {
	matrix.identity();
	matrix.Translate(position.x, position.y, 0);
	matrix.Rotate(rotation*3.1415926 / 180);
	matrix.Scale(scale.x, scale.y, 0);
}

bool Sword::canPlaySound() {
	if (cooldown < 0) {
		return true;
	}
	return false;
}
std::vector<Vect> Sword::relPoints() {
	std::vector<Vect> points;
	points.push_back(Vect(-.5*size*aspect, .5*size, 0));
	points.push_back(Vect(.5*size*aspect, .5*size, 0));
	points.push_back(Vect(.5*size*aspect, -.5*size, 0));
	points.push_back(Vect(-.5*size*aspect, -.5*size, 0));
	return points;
}

//returns the global corners of this object
std::vector<Vect> Sword::globPoints() {
	std::vector<Vect> points = relPoints();
	for (Vect& v : points) {
		v = matrix * v;
	}
	return points;
}

void Sword::resetSword() {
	rotation = 0;
	active = false;
}

Sword::~Sword()
{
}
