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

class Enemy
{
public:
	Matrix matrix;
	Vect position;
	float width;
	float height;
	Vect scale;
	float rotation;
	float speed;
	GLuint texture;
	SheetSprite sprite;
	float size;
	float  aspect;
	bool active;

	Enemy();
	Enemy(float x, float y, GLuint *spritep);

	void drawSprite(ShaderProgram &program);

	void updateMatrix();


	void Update(float x, float y, float elapsed);

	void Die();

	std::vector<Vect> relPoints();

	std::vector<Vect> globPoints();

	~Enemy();
};

