#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Vect.h"
#include "Matrix.h"
#include "ShaderProgram.h"
#include "SheetSprite.h"
#include "main.h"
class Entity1
{
public:
	Matrix matrix;
	Vect position;
	Vect scale;
	float rotation;
	float speed;
	GLuint texture;
	SheetSprite sprite;
	float size;

	Entity1();

	Entity1(float xp, float yp, float widthp, float heightp, float sizep, float rotationp, float speedp, GLuint texturep);

	void Draw(ShaderProgram &program, Matrix &modelMatrix);

	void drawSprite(ShaderProgram &program);

	void updateMatrix();

	void Input(float moveX, float moveY, float elapsed);

	void Update();

	std::vector<Vect> relPoints();

	std::vector<Vect> globPoints();

	void collisionAction(GLuint color);

	void collisionMove(Vect p);

	~Entity1();
};

