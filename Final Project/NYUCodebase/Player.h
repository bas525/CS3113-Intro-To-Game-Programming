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
#include "Laser.h"
#include "Sword.h"
class Player
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


	int laserStock;

	Player();
	Player(GLuint *spritep);

	void fireLaser();

	void drawSprite(ShaderProgram &program);

	void drawLasers(ShaderProgram &program);

	void drawSword(ShaderProgram & program);

	std::vector<std::vector<Vect>> laserCoordinates();

	std::vector<bool> activeLasers();

	std::vector<bool> dangerousLasers();

	void setLaserOff(int i);

	void laserBounce(int i);

	void swordAttack();

	std::vector<Vect> swordCord();

	bool swordActive();

	void updateMatrix();

	void Input(float moveX, float moveY, float elapsed);

	void Update(float elapsed);

	std::vector<Vect> relPoints();

	std::vector<Vect> globPoints();

	~Player();
};

