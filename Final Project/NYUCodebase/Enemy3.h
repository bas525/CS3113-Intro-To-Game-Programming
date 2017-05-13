#pragma once
#include "Enemy.h"
class Enemy3 :
	public Enemy
{
public:
	Enemy3();
	Enemy3(float x, float y, float speedp, GLuint * spritep);
	void Update(float x, float y, float elapsed);
	std::vector<Vect> relPoints();
	std::vector<Vect> globPoints();
	~Enemy3();
};

