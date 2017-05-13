#include "Entity1.h"



Entity1::Entity1()
{
	rotation = 0;
	speed = 1;
	size = 0;
}

Entity1::Entity1(float xp, float yp, float widthp, float heightp, float sizep, float rotationp, float speedp, GLuint texturep) {
	position = Vect(xp, yp, 0);
	scale = Vect(widthp*sizep, heightp*sizep, 0);
	rotation = rotationp;
	speed = speedp;
	texture = texturep;
	updateMatrix();
	size = 0;
}

void Entity1::Draw(ShaderProgram &program, Matrix &modelMatrix) {
	updateMatrix();
	program.setModelMatrix(matrix);

	glBindTexture(GL_TEXTURE_2D, texture);

	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);
}

void Entity1::drawSprite(ShaderProgram & program)
{
	updateMatrix();
	program.setModelMatrix(matrix);
	sprite.drawSprite(&program);
}


void Entity1::updateMatrix() {
	matrix.identity();
	matrix.Translate(position.x, position.y, 0);
	matrix.Rotate(rotation*3.1415926 / 180);
	matrix.Scale(scale.x, scale.y, 0);
}

void Entity1::Input(float moveX, float moveY, float elapsed) {
	position.x += moveX * speed * elapsed;
	position.y += moveY * speed * elapsed;
	updateMatrix();
}

void Entity1::Update() {
	updateMatrix();
}

std::vector<Vect> Entity1::relPoints() {
	std::vector<Vect> points;
	//points.push_back(Vect(-.5, -.5, 0));
	points.push_back(Vect(-.5, .5, 0));
	points.push_back(Vect(.5, .5, 0));
	points.push_back(Vect(.5, -.5, 0));
	points.push_back(Vect(-.5, -.5, 0));
	return points;
}

//returns the global corners of this object
std::vector<Vect> Entity1::globPoints() {
	std::vector<Vect> points = relPoints();
	for (Vect& v : points) {
		v = matrix * v;
	}
	return points;
}

void Entity1::collisionAction(GLuint color) {
	texture = color;
}

void Entity1::collisionMove(Vect p) {
	position.x += p.x / 2;
	position.y += p.y / 2;
}

Entity1::~Entity1()
{
}
